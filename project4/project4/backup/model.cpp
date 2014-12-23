#include <memory.h>
#include <math.h>
#include <stdio.h>
#include "model.h"

namespace train {

	void Model::init(int temp_len[], double templates[][max_x][feat_num]) {
		// initialize segment
		memset(seg_size, 0, sizeof(seg_size));
		seg_size[0] = train_temp_num; // how many points in dummy segment 
		for (int k = 0; k < train_temp_num; ++k) {
			seg_range[k][0][0] = seg_range[k][0][1] = 0;
			int seg_len = temp_len[k] / (state_num - 1);
			int start = 1, end = start + seg_len - 1;
			for (int i = 1; i < state_num; ++i) {
				seg_size[i] += seg_len;
				seg_range[k][i][0] = start;
				seg_range[k][i][1] = end;
				start = end + 1;
				end = start + seg_len - 1;
			}
			seg_range[k][state_num - 1][1] += temp_len[k] % (state_num - 1);
			seg_size[state_num - 1] += temp_len[k] % (state_num - 1);
		}

		for (int i = 0; i < state_num; ++i) {
			int possible_way = 1;
			if (i + 1 < state_num) possible_way++;
			if (i + 2 < state_num) possible_way++;
			double prob = -log((double)1 / possible_way);
			for (int j = 0; j < possible_way; ++j) {
				trans[i][i + j] = prob;
			}
		}
	}

	/**
	 * update the state translation probability
	 */
	void Model::updateTrans(double templates[][max_x][feat_num]) {
		for (int i = 0; i < state_num; ++i) {
			int to_next = 0, to_next_two = 0;
			for (int j = 0; j < train_temp_num; ++j) {
				if (seg_range[j][i][0] == -1) continue;
				if (i + 1 < state_num && seg_range[j][i + 1][0] != -1)
					to_next++;
				else if (i + 2 < state_num && seg_range[j][i + 2][0] != -1)
					to_next_two++;
			}
			double self_prob = (double)(seg_size[i] - to_next - to_next_two) / seg_size[i];
			double next_prob = (double)to_next / seg_size[i];
			double next_two_prob = (double)to_next_two / seg_size[i];
			trans[i][i] = (self_prob == 0 ? inf : -log(self_prob));
			if (i + 1 < state_num) trans[i][i + 1] = (next_prob == 0 ? inf : -log(next_prob));
			if (i + 2 < state_num) trans[i][i + 2] = (next_two_prob == 0 ? inf : -log(next_two_prob));
		}	 
	}

	/**
	 * update segmentation according to the match result
	 */
	void Model::updateSegment(const int &k, const int &k_len, int arrow[][max_y]) {
		for (int s = 0; s < state_num; ++s) seg_range[k][s][0] = -1;
		int i = k_len, j = state_num - 1; // i means current state
		seg_range[k][j][1] = i;
		while (i > 0 || j > 0) {
			if (arrow[i][j] == 0) { // horizontal arrow
				--i;
			}
			else if (arrow[i][j] == 1) { // diagonal arrow 1
				seg_range[k][j][0] = i; 
				seg_size[j] += seg_range[k][j][1] - seg_range[k][j][0] + 1;
				--i; 
				--j; // state translate
				seg_range[k][j][1] = i; // next state range
			}
			else if (arrow[i][j] == 2) { // diagonal arrow 1
				seg_range[k][j][0] = i;
				seg_size[j] += seg_range[k][j][1] - seg_range[k][j][0] + 1;
				--i; 
				j-=2; // state translate
				seg_range[k][j][1] = i; // next state range
			}
		}
		// at this moment i = 0 and j = 0, which is dummy state
		seg_range[k][j][0] = i;
		seg_size[j] += seg_range[k][j][1] - seg_range[k][j][0] + 1;
	}

	void SGModel::init(int temp_len[], double templates[][max_x][feat_num]) {
		Model::init(temp_len, templates);
		updateMeansAndCov(templates);
	}

	/**
	 * update means and variances for each state
	 */
	void SGModel::updateMeansAndCov(double templates[][max_x][feat_num]) {
		for (int i = 1; i < state_num; ++i) {
			for (int j = 0; j < feat_num; ++j) {
				double sum = 0, sqare_sum = 0;
				for (int k = 0; k < train_temp_num; ++k) {
					if (seg_range[k][i][0] == -1)
						continue;
					for (int l = seg_range[k][i][0]; l <= seg_range[k][i][1]; ++l) {
						sum += templates[k][l][j];
						sqare_sum += templates[k][l][j] * templates[k][l][j];
					}
				}
				means[i][j] = sum / seg_size[i];
				covar[i][j] = sqare_sum / seg_size[i] - means[i][j] * means[i][j];
			}
		}
	}

	double SGModel::calcNodeCost(double input_vector[feat_num], const int &state) {
		if (state == 0) return inf;
		double sum1 = 0, sum2 = 0;
		for (int n = 0; n < feat_num; ++n) {
			double sigma_square = covar[state][n];
			sum1 += log(2 * Pi * sigma_square); // calc log 2*pi*sigma
			sum2 += (input_vector[n] - means[state][n]) * (input_vector[n] - means[state][n]) / sigma_square;
		}
		return 0.5 * (sum1 + sum2);
	}

	void GMModel::init(int temp_len[], double templates[][max_x][feat_num]) {
		Model::init(temp_len, templates);
		updateMeansAndCov(templates);
	}

	void GMModel::updateMeansAndCov(double templates[][max_x][feat_num]) {
		for (int i = 1; i < state_num; ++i) {
			//printf("stateaaaaaaaaaaaaaaaa: %d\n", i);
			int cur_num = 1; // current GMM cluster number  
			for (int j = 0; j < feat_num; ++j) {
				double sum = 0, sqare_sum = 0;
				for (int k = 0; k < train_temp_num; ++k) {
					if (seg_range[k][i][0] == -1)
						continue;
					for (int l = seg_range[k][i][0]; l <= seg_range[k][i][1]; ++l) {
						sum += templates[k][l][j];
						sqare_sum += templates[k][l][j] * templates[k][l][j];
					}
				}
				means[i][0][j] = sum / seg_size[i];
				covar[i][0][j] = sqare_sum / seg_size[i] - means[i][0][j] * means[i][0][j];
			}
			weights[i][0] = 1;
			while (cur_num < cluster_num) {
				// split
				/*for (int k = 0; k < cur_num; ++k) {
					for (int j = 0; j < feat_num; ++j) {
						means[i][k + cur_num][j] = means[i][k][j] + EPSILON;
						means[i][k][j] -= EPSILON;
						covar[i][k + cur_num][j] = covar[i][k][j];
					}
					weights[i][k] /= 2;
					weights[i][k + cur_num] = weights[i][k];
				}
				cur_num *= 2;*/
				double max_var = 0;
				int max_c;
				// find the cluster with the largest co-variance
				for (int c = 0; c < cur_num; ++c) {
					double var = 1;
					//if (i == 3)
						//printf("aaaaaaaaaa %d\n", seg_size[3]);
					for (int j = 0; j < feat_num; ++j) {
						var *= fabs(covar[i][c][j]);
					}
					if (var > max_var) {
						max_var = var;
						max_c = c;
					}
				}
				//printf("%lf %d %d\n", max_var, max_c, cur_num);
				// split the cluster with the largest co-variance
				for (int j = 0; j < feat_num; ++j) {
					//printf("%lf\n", means[i][max_c][j]);
					means[i][cur_num][j] = means[i][max_c][j] + EPSILON;
					means[i][max_c][j] -= EPSILON;
					covar[i][cur_num][j] = covar[i][max_c][j];
				}
				weights[i][max_c] /= 2;
				weights[i][cur_num] =  weights[i][max_c];
				cur_num++;
				// k-means
				kMeansOnState(i, cur_num, templates);
			}
		}
	}

	double GMModel::calcNodeCost(double input_vector[feat_num], const int &state) {
		if (state == 0) return inf;
		double result = 0;
		for (int c = 0; c < cluster_num; ++c) {
			double sum1 = 1, sum2 = 0;
			for (int n = 0; n < feat_num; ++n) {
				double sigma_square = covar[state][c][n];
				sum1 *= 2 * Pi * sigma_square; // calc 2*pi*sigma
				sum2 += (input_vector[n] - means[state][c][n]) * (input_vector[n] - means[state][c][n]) / sigma_square;
			}
			//if (state == 3) {
				//printf("cluster %d, weight: %lf, sum1: %lf, sum2: %lf, result: %lf\n", c, weights[state][c], sum1, sum2, weights[state][c] / sqrt(sum1) * pow((double)E, (-0.5) * sum2));
			//}
			result += weights[state][c] / sqrt(sum1) * pow((double)E, (-0.5) * sum2); 
		}
		//if (state == 3)
			//printf("%lf\n", -log(result));
		return -log(result);
	}

	double GMModel::calcClusterDist(double input_vector[feat_num], const int &state, const int &c_index) {
		double sum1 = 0, sum2 = 0;
		for (int n = 0; n < feat_num; ++n) {
			double sigma_square = covar[state][c_index][n];
			sum1 += log(2 * Pi * sigma_square); // calc log 2*pi*sigma
			sum2 += (input_vector[n] - means[state][c_index][n]) * (input_vector[n] - means[state][c_index][n]) / sigma_square;
		}
		return 0.5 * (sum1 + sum2) - log(weights[state][c_index]);
	}

	void GMModel::kMeansOnState(const int &state, const int &cur_num, double templates[][max_x][feat_num]) {
		int iter_time = 0;
		while (1) {
			double total_diff = 0;
			double temp_means[cluster_num][feat_num] = {0};
			double temp_covar[cluster_num][feat_num] = {0};
			int cluster_size[cluster_num] = {0};
			for (int k = 0; k < train_temp_num; ++k) {
				for (int l = seg_range[k][state][0]; l <= seg_range[k][state][1]; ++l) {
					if (seg_range[k][state][0] == -1) continue;
					double min_dist = inf; int min_c;
					for (int c = 0; c < cur_num; ++c) {
						double dist = calcClusterDist(templates[k][l], state, c);
						//if (state == 2)
							//printf("cluster: %d, dist: %lf\n", c, dist);
						if (dist < min_dist) {
							min_dist = dist;
							min_c = c;
						}
					}
					cluster_size[min_c]++;
					for (int j = 0; j < feat_num; ++j) {
						temp_means[min_c][j] += templates[k][l][j];
						temp_covar[min_c][j] += templates[k][l][j] * templates[k][l][j];
					}
				}
			}
			for (int c = 0; c < cur_num; ++c) {
				//if (state == 2)
					//printf("cluster: %d, size: %d\n", c, cluster_size[c]);
				double temp_weights = weights[state][c];
				weights[state][c] = (double)cluster_size[c] / seg_size[state];
				total_diff += fabs(weights[state][c] - temp_weights);
				for (int j = 0; j < feat_num; ++j) {
					means[state][c][j] = temp_means[c][j] / cluster_size[c];
					covar[state][c][j] = temp_covar[c][j] / cluster_size[c] - means[state][c][j] * means[state][c][j];
				}
			//if (state == 2)
			//printf("iter: %d, cur_cluster_num: %d, cluster %d: %lf, size: %d\n", iter_time, cur_num, c, covar[state][c][0], cluster_size[c]);
			}
			if (iter_time > 5 && total_diff == 0)
				break;
			++iter_time;
		}
	}

}