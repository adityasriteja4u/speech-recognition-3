#include <stdio.h>
#include <memory.h>
#include <string>
#include <algorithm>
using namespace std;

const int string_num = 5;
const int beam = 100;
const int maxlen = 1000;
int dp[maxlen][maxlen];
string strs[5] = {"FUCK YOU", "Elephant", "Elegant", "Sycophant", "SHINE"};
int strlength[string_num];



// arrow[i][j] stores where the value comes from, 
// 0 means horizontal arrow, 1 means vertical arrow, 2 means diagonal arrow
int arrow[maxlen][maxlen]; 

/**
 * output trellis
 */
void outputTrellis(const int &alen, const int &blen, int rowindex)
{
	printf("=== Trellis ===\n");
	for (int j = rowindex+alen; j >= rowindex; --j) {
		for (int i = 0; i <= blen; ++i)
			printf("%d\t", dp[i][j]);
		printf("\n");
	}
}

/**
 * output best path
 */
void outputBestPath(const int &alen, const int &blen, int rowindex)
{
	for (int i = 0; i <= blen; ++i)
		arrow[i][0] = 0;
	for (int j = rowindex; j <= rowindex+alen; ++j)
		arrow[0][j] = 1;

	printf("=== Best Path ===\n");
	int path[maxlen], pathlen = 0;
	int i = blen, j = alen;
	while (i != 0 || j != 0) {
		path[pathlen++] = arrow[i][j];
		if (arrow[i][j] == 0) {
			--i;
		}
		else if (arrow[i][j] == 1) {
			--j;
		}
		else {
			--i; --j;
		}
	}
	for (int p = pathlen - 1; p >= 0; --p)
		printf("%d ", path[p]);
	printf("\n");
}

/**
 * get the min number among a, b and c. arg idx is the index of the smallest one
 */
int getMin(const int &a, const int &b, const int &c, int &idx)
{
	int min;
	if (a <= b && a <= c) {
		min = a;
		idx = 0;
	}
	else if (b < a && b <= c) {
		min = b;
		idx = 1;
	}
	else if (c < a && c < b) {
		min = c;
		idx = 2;
	}
	return min;
}



/**
 * update next column of the dp array according to current value dp[i][j]
 * at the same time update the min cost of the next column.
 */
void updateNextCol(const int &i, const int &j, const string &a, const string &b, int &min_cost, int rowindex, int flag = 0)
{
	if (flag) {
		if (j < rowindex+a.length() && dp[i + 1][j] != -1)
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
	if (j < a.length()+rowindex) {
		int temp = dp[i][j] + (a[j-rowindex] == b[i] ? 0 : 1);
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
 * calculate min edit distance with beam search pruning
 */
void minEditDistP(const string *a, const string &b)
{
	memset(dp, -1, sizeof(dp));
	int alen;
	int blen = b.length();
	int min_cost = 0; // min cost of the next column.
	int rowindex = 0;
	for (int k = 0; k < string_num; ++k)
		strlength[k] = strs[k].length();
	for (int i = 0; i < blen; ++i) {
		// get the min cost of this column
		int cost = min_cost; min_cost = 9999;
		for (int k = 0; k < string_num; ++k){
			for (int j = rowindex; j <= rowindex+strlength[k]; ++j) {
				if (i == 0)
					dp[i][j] = j-rowindex;
				if (dp[i][j] != -1) {
					if (dp[i][j] > cost + beam) {
						dp[i][j] = -1;
						updateNextCol(i, j, strs[k], b, min_cost, rowindex, 1);
					}
					else 
						updateNextCol(i, j, strs[k], b, min_cost, rowindex);
				}
				else { // if dp[i][j] is -1, update next column in a different way
					updateNextCol(i, j, strs[k], b, min_cost, rowindex, 1);
				}
			}
		rowindex+=(strlength[k]+1);
		}
		rowindex = 0;
	}
	
	rowindex = 0;
	for (int k = 0; k < string_num; ++k)
	{
		alen = strs[k].length();
		outputTrellis(alen, blen, rowindex);
		outputBestPath(alen, blen, rowindex);
		printf("cost: %d\n", dp[blen][alen+rowindex]);
		printf("%d %d %d\n\n",blen,strs[k].length(), alen+rowindex);
		rowindex+=(alen+1);
	
	}
}


int main()
{
	
	minEditDistP(strs, "Elephant");

	return 0;
}