#ifndef _MIN_EIDT_DIST_H_
#define _MIN_EIDT_DIST_H_

#include <string>
#include <vector>

typedef struct MatchData {
	int cost, cost_rep, cost_ins, cost_del;
	int best_index;
	std::vector<std::string> best_match;

	MatchData(const int &_cost, const int &_cost_rep, const int &_cost_ins, const int &_cost_del):
			 cost(_cost),
		     cost_rep(_cost_rep),
			 cost_ins(_cost_ins),
			 cost_del(_cost_del)
	{}

	std::string getBestMatch() {
		std::string ret;
		int l = best_match.size();
		for (int i = 0; i < l; i++)
			ret.push_back(best_match[i][0]);
		return ret;
	}
} MatchData;

/**
 * output dp trellis of the specific template string and input string
 */
void outputEachTrellis(const int &templen, const int &inputlen, const int &tempindex);

/**
 * output best path
 */
void outputBestPath(const int &templen, const int &inputlen, const int &tempindex);

/**
 * output data for each template string
 */
void outputTrellisAndBestPath(const MatchData &data);

/**
 * backtrace
 */
void backTrace(const int &templen, const int &inputlen, const int &k, MatchData &data);

/**
 * update next column of the dp array according to current value dp[i][j]
 * at the same time update the min cost of the next column.
 */
void updateNextCol(const int &i, const int &j, const int &k, int &next_min_cost, int flag = 0);

/** 
 * update column i for template k, param cost is the smallest cost of this column. 
 * at the same time update min_cost of the next column
 */
void updateColForTempK(const int &i, const int &k, const int &cur_min_cost, int &next_min_cost, MatchData &match_data);

/**
 * calculate min edit distance with beam search pruning
 */
MatchData minEditDistPrune();

/**
 * initialize functions
 */
void initTemplates(const std::vector<std::string>& template_string, const int &index);

void initTemplates(const std::string& template_string, const int &index);

void initInput(const std::string& input_string);

void initInput(const std::vector<std::string>& input_string);

/**
 * other functions used privately
 */
void init();

void assign(std::vector<std::string> &obj, const std::string &source);

#endif