#ifndef _LEX_TREE_H_
#define _LEX_TREE_H_

#include <cstdio>
#include <ctime>
#include <fstream>

//#define _DEBUG
namespace lex {

const int max_son_num = 52;
const int max_word_len = 30;
const int max_word_num = 7000;
//const char* dict_path = "dict.txt";
const char* Lextrees_path = "lextrees.bin";
const int max_node_num = 30000;

char Lextrees[max_node_num];
int row_num_of_son[max_node_num][max_son_num];
int row_num_of_father[max_node_num];
int row_num_of_word[max_word_num];
int son_num_of_node[max_node_num];
int word_num_of_row[max_node_num];

int node_num = 1;
int count = 0;
int word_num = 0;

struct Trie_node
{
	bool is_str;
	int son_num;
	int word_index;
	struct Trie_node *next[max_son_num];
};

void insert(Trie_node *root, const char *s, int &index)
{
	if (root == NULL || *s=='\0')
		return;
	int i;
	Trie_node *p = root;
	Trie_node *parent = NULL;
	while (*(s+1) != '\0')
	{
		if (p->next[*s-'a'] == NULL)
		{
			Trie_node *temp = new Trie_node;
			temp->son_num = 0;
			++(p->son_num);
			++node_num;
			for (i = 0; i < max_son_num; ++i)
				temp->next[i] = NULL;
			temp->is_str = false;
			p->next[*s-'a'] = temp;
			p = p->next[*s-'a'];
		}
		else
			p = p->next[*s-'a'];
		++s;
	}
	if (p->next[*s-'a'+26] == NULL)
	{
		Trie_node *temp = new Trie_node;
		temp->son_num = 0;
		++node_num;
		for (i = 0; i < max_son_num; ++i)
			temp->next[i] = NULL;
		temp->is_str = true;
		temp->word_index = index;
		p->next[*s-'a'+26] = temp;
		++p->son_num;
	}
	else
		row_num_of_word[index] = -1;  // duplicated words

}

int search(Trie_node *root, const char *s)
{
	Trie_node *p = root;
	while (p != NULL && *s!='\0')
	{
		p = p->next[*s-'a'];
		++s;
	}
	if (p!=NULL && p->is_str == true)
		return p->word_index;
	else
		return -2;
}

void del(Trie_node *root)
{
	int i;
	for (i = 0; i < max_son_num; ++i)
	{
		if (root->next[i] != NULL)
			del(root->next[i]);
	}
	delete root;
}


void dealWithWord(char* word)
{
	char* p = word;
	char* q = word;
	while (*q) {
		if ((*q) >= 'A' && (*q) <= 'Z') 
			*(p++) = (*q) | 0x20;
		else if ((*q) >= 'a' && (*q) <= 'z')
			*(p++) = *q;
		++q;
	}
	*p = '\0';
}


void buildTrie(const char* filename, Trie_node* root)
{
	FILE* file = fopen(filename, "r");
	char temp[max_word_len];
	int index = 0;
	while (fscanf(file, "%s", temp) == 1)  
	{
		dealWithWord(temp);
		insert(root, temp, index);
		++index;
	}
	word_num = index;
	fclose(file);
}

void getLexPara(Trie_node* root, int next_index, int father_row_num, int son_index)
{
	if (father_row_num == -1)
	{
		Lextrees[0] = '*';
		row_num_of_father[0] = -1;
	}
	else
	{
		if (next_index >= 26) next_index -= 26;
		Lextrees[++count] = next_index + 'a';
		row_num_of_son[father_row_num][son_index] = count;
		row_num_of_father[count] = father_row_num;
		if (root->is_str == true)
		{
			row_num_of_word[root->word_index] = count;
			word_num_of_row[count] = root->word_index;
		}
	}
	int root_row_num = count;
	son_num_of_node[count] = root->son_num;
	int _son_index = 0;
	for (int i = 0; i < max_son_num; ++i)
	{
		if (root->next[i] != NULL)
		{
			getLexPara(root->next[i], i, root_row_num, _son_index);
			++_son_index;	
		}
	}
}

void writeLextrees()
{
	FILE *fp;
	if ((fp = fopen(Lextrees_path, "wb")) == NULL)
	{
		printf("Unable to open file\n");
		return;
	}
	fwrite(&word_num, sizeof(int), 1, fp);
	fwrite(&node_num, sizeof(node_num),1, fp);
	fwrite(Lextrees, sizeof(char), node_num, fp);
	fwrite(row_num_of_son, sizeof(int), node_num*max_son_num, fp);
	fwrite(word_num_of_row, sizeof(int), node_num, fp);
	fwrite(row_num_of_father, sizeof(int), node_num, fp);
	fwrite(row_num_of_word, sizeof(int), word_num, fp);
	fwrite(son_num_of_node, sizeof(int), node_num, fp);

	fclose(fp);
}

void readLextrees()
{
	FILE *fp;
	if ((fp = fopen(Lextrees_path, "rb")) == NULL)
	{
		printf("Unable to open file\n");
		return;
	}
	fread(&word_num, sizeof(int), 1, fp);
	fread(&node_num, sizeof(node_num),1, fp);
	fread(Lextrees, sizeof(char), node_num, fp);
	fread(row_num_of_son, sizeof(int), node_num*max_son_num, fp);
	fread(word_num_of_row, sizeof(int), node_num, fp);
	fread(row_num_of_father, sizeof(int), node_num, fp);
	fread(row_num_of_word, sizeof(int), word_num, fp);
	fread(son_num_of_node, sizeof(int), node_num, fp);

	fclose(fp);
}

Trie_node* createTree(const char *dict_path) 
{
	Trie_node *root = new Trie_node;
	root->son_num = 0;
	for (int i = 0; i < max_son_num; ++i)
		root->next[i] = NULL;
	root->is_str = false;
	buildTrie(dict_path, root);
	//printf("%d\n",node_num);
	getLexPara(root, -1, -1, -1);
	writeLextrees();

	return root;
}

}
#endif

//void main()
//{
//	int i = 0;
//	lex::Trie_node *root = new lex::Trie_node;
//	root->son_num = 0;
//	for (i = 0; i < lex::max_son_num; ++i)
//		root->next[i] = NULL;
//	root->is_str = false;
//	build_Trie(lex::dict_path, root);
//	printf("%d\n",lex::node_num);
//	get_Lex_para(root, -1, -1, -1);
//	lex::write_Lextrees();
//
//	clock_t start, end;
//	start = clock();
//	lex::read_Lextrees();
//	end = clock();
//	printf("total time: %.8f\n", (double)(end - start)/CLOCKS_PER_SEC);
//
//	printf("%d\n",lex::node_num);
//	//printf("%d\n",search(root,"abando"));
//}
