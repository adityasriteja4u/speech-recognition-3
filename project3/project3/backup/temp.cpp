#include <stdio.h>
#include <memory.h>
#include <string>
#include <algorithm>
using namespace std;

const int beam = 3;
const int maxlen = 1000;
int dp[maxlen][maxlen];
// arrow[i][j] stores where the value comes from, 
// 0 means horizontal arrow, 1 means vertical arrow, 2 means diagonal arrow
int arrow[maxlen][maxlen]; 

/**
 * output trellis
 */
void outputTrellis(const int &alen, const int &blen)
{
	printf("=== Trellis ===\n");
	for (int j = alen; j >= 0; --j) {
		for (int i = 0; i <= blen; ++i)
			printf("%d\t", dp[i][j]);
		printf("\n");
	}
}

/**
 * output best path
 */
void outputBestPath(const int &alen, const int &blen)
{
	for (int i = 0; i <= blen; ++i)
		arrow[i][0] = 0;
	for (int j = 0; j <= alen; ++j)
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
 * calculate min edit distance without pruning
 */
int minEditDist(const string &a, const string &b)
{
	int dp_temp; // store dp state for future use
	int alen = a.length(), blen = b.length();
	// initialize dp array and arrow array
	for (int i = 0; i <= blen; ++i) 
		dp[i][0] = i;
	for (int j = 0; j <= alen; ++j)
		dp[0][j] = j;

	// update trellis
	for (int i = 1; i <= blen; ++i) {
		for (int j = 1; j <= alen; ++j) {
			int diag = dp[i-1][j-1] + (a[j-1] == b[i-1] ? 0 : 1);
			dp[i][j] = getMin(dp[i-1][j] + 1, dp[i][j-1] + 1, diag, arrow[i][j]);
		}
	}
	outputTrellis(alen, blen);
	outputBestPath(alen, blen);
	return dp[blen][alen];
}

/**
 * update next column of the dp array according to current value dp[i][j]
 * at the same time update the min cost of the next column.
 */
void updateNextCol(const int &i, const int &j, const string &a, const string &b, int &min_cost, int flag = 0)
{
	if (flag) {
		if (j < a.length() && dp[i + 1][j] != -1)
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
	if (j < a.length()) {
		int temp = dp[i][j] + (a[j] == b[i] ? 0 : 1);
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
int minEditDistP(const string &a, const string &b)
{
	memset(dp, -1, sizeof(dp));
	int alen = a.length(), blen = b.length();
	int min_cost = 0; // min cost of the next column.

	for (int i = 0; i < blen; ++i) {
		// get the min cost of this column
		int cost = min_cost; min_cost = 9999; 
		for (int j = 0; j <= alen; ++j) {
			if (i == 0)
				dp[i][j] = j;
			if (dp[i][j] != -1) {
				if (dp[i][j] > cost + beam) {
					dp[i][j] = -1;
					updateNextCol(i, j, a, b, min_cost, 1);
				}
				else 
					updateNextCol(i, j, a, b, min_cost);
			}
			else { // if dp[i][j] is -1, update next column in a different way
				updateNextCol(i, j, a, b, min_cost, 1);
			}
		}
	}
	outputTrellis(alen, blen);
	outputBestPath(alen, blen);
	return dp[blen][alen];
}

int main()
{
	int cost = minEditDistP("elephantaaaaaaaaaaaaaaaaaa", "alephent");
	printf("cost: %d\n", cost);
	return 0;
}