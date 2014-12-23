#ifndef _LANG_MODEL_H_
#define _LANG_MODEL_H_

const int max_word_num = 1000;
const char *dict_path = "dict.txt";

class LangModel {
public:
	void init(const char *path);
private:
	int total_number;
	int word_freq[max_word_num];
	
	void dealWithWord(char* word);
};

#endif