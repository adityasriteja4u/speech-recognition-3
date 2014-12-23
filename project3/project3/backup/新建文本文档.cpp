void updateNextCol(const int &i, const int &j, const int &k, int &next_min_cost, int flag)
{
	if (flag) {
		if (j == range[k][0]) { dp[i + 1][j] = -1; }
		else if (dp[i + 1][j - 1] == -1) { dp[i + 1][j] = -1; }
		else if (dp[i + 1][j - 1] + 1 < dp[i + 1][j]){ dp[i + 1][j] = dp[i + 1][j - 1] + 1; arrow[i + 1][j] = 1; }
		return;
	}

	// update the entry on the right
	if (j == range[k][0] || dp[i + 1][j] == -1 || dp[i][j] + 1 < dp[i + 1][j]) {
		dp[i + 1][j] = dp[i][j] + 1;
		arrow[i + 1][j] = 0;
		if (dp[i + 1][j] < next_min_cost)
			next_min_cost = dp[i + 1][j];
	}
	// update the entry on upper right
	if (j < range[k][1]) {
		int temp = dp[i][j] + (strcmp(templates[j], inputstr[i]) == 0 ? 0 : 1);
	
		if (dp[i + 1][j] == -1 || temp < dp[i + 1][j] + 1) {
			dp[i + 1][j + 1] = temp; arrow[i + 1][j + 1] = 2;
		}
		else {
			dp[i + 1][j + 1] = dp[i + 1][j] + 1; arrow[i + 1][j + 1] = 1;
		}
		if (dp[i + 1][j + 1] < next_min_cost)
			next_min_cost = dp[i + 1][j + 1];
	}
}


if (i == 0) {
	for (int j = 0; j <= alen; ++j) {
		if (j <= beam) {
			dp[i][j] = j;
			updateNextCol(i, j, a, b, min_cost);
		}
	}
}
else {
	// get the min cost of this column
	int cost = min_cost; min_cost = 9999; 
	for (int j = 0; j <= alen; ++j) {
		if (dp[i][j] != -1) {
			if (dp[i][j] > cost + beam)
				dp[i][j] = -1;
			else 
				updateNextCol(i, j, a, b, min_cost);
		}
	}
}

#include <stdio.h>
#include <memory.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include "min_edit_dist.h"
using namespace std;

// parameters
const int beam = 3;
const int max_x = 1000;
const int max_y = 100000;
const int max_temp_num = 7000;

// global variables
int dp[max_x][max_y];
int arrow[max_x][max_y]; // 0 means horizontal arrow, 1 means vertical arrow, 2 means diagonal arrow
int temp_num; // store the actual number of templates
int templength[max_temp_num]; // store length of each temp
int rowindex[max_temp_num]; // store the start index of each temp
bool abandon[max_temp_num]; // whether to abandon a template 
vector<string> templates[max_temp_num];
vector<string> inputstr;
vector<string> curtempstr;

/**
 * output dp trellis of the specific template string and input string
 */
void outputEachTrellis(const int &templen, const int &inputlen, const int &k)
{
	for (int j = rowindex[k] + templen; j >= rowindex[k]; --j) {
		//if (j != rowindex) printf("%c\t", templates[tempindex][j - rowindex - 1]);
		//else printf("0\t");

		for (int i = 0; i <= inputlen; ++i)
			printf("%2d\t", dp[i][j]);
		printf("\n");
	}

	// print input string
	/*if (tempindex != temp_num - 1)
		return;
	printf(" \t 0\t");
	for (int i = 0; i < inputlen; ++i)
		printf("%2c\t", inputstr[i]);
	printf("\n");*/
}

/**
 * output best path
 */
//void outputBestPath(const int &templen, const int &inputlen)
//{
//	for (int i = 0; i <= inputlen; ++i)
//		arrow[i][0] = 0;
//	for (int j = rowindex; j <= rowindex + templen; ++j)
//		arrow[0][j] = 1;
//
//	printf("=== Best Path ===\n");
//	int path[max_y], pathlen = 0;
//	int i = inputlen, j = templen;
//	while (i != 0 || j != 0) {
//		path[pathlen++] = arrow[i][j];
//		if (arrow[i][j] == 0) { --i; }
//
//		else if (arrow[i][j] == 1) { --j; }
//
//		else { --i; --j; }
//	}
//
//	for (int p = pathlen - 1; p >= 0; --p)
//		printf("%d ", path[p]);
//	printf("\n\n");
//}

/**
 * output data for each template string
 */
void outputDataForEachTemp()
{
	int inputlen = inputstr.size();

	printf("=== Trellis ===\n");
	for (int k = 0; k < temp_num; ++k)
	{
		int templen = templates[k].size();
		outputEachTrellis(templen, inputlen, k);
	}
}

/**
 * update next column of the dp array according to current value dp[i][j]
 * at the same time update the min cost of the next column.
 */
void updateNextCol(const int &i, const int &j, const int &k, int &min_cost, int flag)
{
	if (flag) {
		if (j < rowindex[k] + curtempstr.size() && dp[i + 1][j] != -1)
			dp[i + 1][j + 1] = dp[i + 1][j] + 1; arrow[i + 1][j + 1] = 1;
		return;
	}

	// update the entry on the right
	if (dp[i + 1][j] == -1 || dp[i][j] + 1 < dp[i + 1][j]) {
		dp[i + 1][j] = dp[i][j] + 1;
		arrow[i + 1][j] = 0;
		if (dp[i + 1][j] < min_cost)
			min_cost = dp[i + 1][j];
	}
	// update the entry on upper right
	if (j < curtempstr.size() + rowindex[k]) {
		int temp = dp[i][j] + (curtempstr[j - rowindex[k]] == inputstr[i] ? 0 : 1);
		if (temp < dp[i + 1][j] + 1) {
			dp[i + 1][j + 1] = temp; arrow[i + 1][j + 1] = 2;
		}
		else {
			dp[i + 1][j + 1] = dp[i + 1][j] + 1; arrow[i + 1][j + 1] = 1;
		}
		if (dp[i + 1][j + 1] < min_cost)
			min_cost = dp[i + 1][j + 1];
	}
}

/** 
 * update column i for template k, param cost is the smallest cost of this column. 
 * at the same time update min_cost of the next column
 */
void updateColForTempK(const int &i, const int &k, const int &cost, int &min_cost, MatchData &match_data)
{
	bool abandon_k = true;
	curtempstr = templates[k];
	for (int j = rowindex[k]; j <= rowindex[k] + templength[k]; ++j) {
		if (i == 0)
			dp[i][j] = j - rowindex[k];
		if (dp[i][j] != -1) {
			if (dp[i][j] > cost + beam) {
				dp[i][j] = -1;
				updateNextCol(i, j, k, min_cost, 1);
			}
			else {
				abandon_k = false;
				updateNextCol(i, j, k, min_cost);
			}
		}
		else { // if dp[i][j] is -1, update next column in a different way
			updateNextCol(i, j, k, min_cost, 1);
		}
	}
	abandon[k] = abandon_k;
	
	// update min edit dist among all the templates as well as the index of the closest template
	if (i == inputstr.size() - 1 && dp[i + 1][rowindex[k + 1] - 1] != -1 && dp[i + 1][rowindex[k + 1] - 1] < match_data.cost)
	{
		match_data.cost = dp[i + 1][rowindex[k + 1] - 1];
		match_data.best_index = k;
		match_data.best_temp = templates[k];
	}
}

/**
 * calculate min edit distance with beam search pruning
 */
MatchData minEditDistPrune()
{
	init();
	int inputlen = inputstr.size();
	int min_cost = 0; // min cost of the next column.
	MatchData match_data; match_data.cost = 9999;

	for (int i = 0; i < inputlen; ++i) {
		int cost = min_cost; min_cost = 9999; // get the min cost of this column
		for (int k = 0; k < temp_num; ++k){ // for each template string
			if (abandon[k]) continue;
			updateColForTempK(i, k, cost, min_cost, match_data);
		}
	}

	// outputDataForEachTemp();
	return match_data;
}

/**
 * initialize
 */
void initTemplates(const char* filename)
{
	int i = 0;
	temp_num = 0;
	// read templates
	ifstream file(filename);
	string temp;
	while (file >> temp) { assign(templates[i++], temp); ++temp_num;}
	file.close();
}

void initTemplates(const std::vector<std::string>& template_string)
{
	templates[0] = template_string;
	temp_num = 1;
}

void initInput(const string& input_string)
{
	assign(inputstr, input_string);
}

void initInput(const std::vector<std::string>& input_string)
{
	inputstr = input_string;
}

void init()
{
	memset(dp, -1, sizeof(dp));
	memset(abandon, 0, sizeof(abandon));

	rowindex[0] = 0;
	for (int k = 0; k < temp_num; ++k) {
		templength[k] = templates[k].size();
		rowindex[k + 1] = rowindex[k] + templength[k] + 1;
	}
}

void assign(vector<string> &obj, const string &source)
{
	int l = source.length();
	obj.clear();
	for (int i = 0; i < l; ++i) {
		string str;
		str.push_back(source[i]);
		obj.push_back(str);
	}
}

