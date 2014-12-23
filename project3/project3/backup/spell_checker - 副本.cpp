#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "min_edit_dist.h"
using namespace std;

const char* DFILENAME = "dict.txt";
const char* SFILENAME = "story.txt";
const char* MSFILENAME = "mystory.txt";
const char* CSFILENAME = "storycorrect.txt";

vector<string> story;
vector<string> correct_story;

string dealWithWord(const string& word)
{
	string result;
	int l = word.length();
	for (int i = 0; i < l; ++i)
	{
		if (word[i] >= 'a' && word[i] <= 'z')
			result.push_back(word[i]);
		else if (word[i] >= 'A' && word[i] <= 'Z')
			result.push_back(word[i]|0x20);
	}
	return result;
}

void readStory(const char* filename, vector<string> &story)
{
	ifstream story_file(filename);
	string story_word;
	while (story_file >> story_word) {
		story.push_back(dealWithWord(story_word));
	}
	story_file.close();
}

void readTemplates(const char* filename)
{
	int i = 0;
	ifstream file(filename);
	string temp;
	while (file >> temp) { initTemplates(temp, i); ++i; }
	file.close();
}

void spellCheckAndWriteFile()
{
	int size = story.size();
	ofstream my_story_file(MSFILENAME);
	for (int i = 0; i < size; ++i) {
		printf("%d\n", i);
		initInput(story[i]);
		//initInput("Gramadatta");
		MatchData data = minEditDistPrune();
		
		my_story_file << data.getBestMatch();
		my_story_file << endl;
	}
	my_story_file.close();
}

void doSpellCheck()
{
	readStory(SFILENAME, story);
	readTemplates(DFILENAME);
	spellCheckAndWriteFile();
}

void doErrorCount(const char* file1, const char* file2)
{
	vector<string> story1;
	vector<string> story2;
	readStory(file1, story1);
	readStory(file2, story2);
	initTemplates(story1, 0);
	initInput(story2);
	MatchData data = minEditDistPrune();
	backTrace(story1.size(), story2.size(), data.best_index, data);
	printf("errors: %d \n insert: %d\n delete: %d\n replace: %d\n", data.cost, data.cost_ins, data.cost_del, data.cost_rep);
}

void doSimpleWordCheck()
{
	const int n = 2;
	string templates[n] = {"elephant", "elegant"};
	string input = "elephant";
	for (int i = 0; i < n; ++i)
		initTemplates(templates[i], i);
	initInput(input);
	MatchData data = minEditDistPrune();

	cout << "Input:" << input << endl;
	cout << "Best match:" << data.getBestMatch() << endl;
	outputTrellisAndBestPath(data);
}

int main()
{
	//doSimpleWordCheck();
	//doSpellCheck();
	doErrorCount(SFILENAME, CSFILENAME);
	return 0;
}