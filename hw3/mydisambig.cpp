#include <iostream>
#include <fstream>
#include "Ngram.h"
#include "Vocab.h"
using namespace std;

char zy_table[37][2];
char zy2ch_map[37][8000*3];
int ch_num[80];
VocabIndex viterbi_map[8000][70];
double viterbi_prob[8000][70];
int viterbi_path[8000][70];
char viterbi_output[80][3];

int Get_zy_index(const char *pch)
{
	int zy_index = -1;
	for(int i = 0 ; i < 37; i++)
	{
		if(strncmp(pch, zy_table[i], 2) == 0)
		{
			zy_index = i;
			break;
		}
	}
	return zy_index;
}

int Repeated(char *pch)
{
	int zy_index = -1;
	for(int i = 0 ; i < 37; i++)
	{
		if(strncmp(pch, zy_table[i], 2) == 0)
		{
			zy_index = i;
			break;
		}
	}
	return zy_index;
}

void get_ch_num(int zy_index, int count)
{
	if(zy_index == -1){
		ch_num[count] = 1;
	}
	else{
		ch_num[count] = strlen(zy2ch_map[zy_index])/3;
	}
}

void get_viterbi_map(int zy_index, int count, char *pch, Vocab &voc)
{
	if(zy_index == -1)
	{
		viterbi_map[0][count] = voc.getIndex(pch);
	}
	else
	{
		for(int i = 0; i < strlen(zy2ch_map[zy_index])/3; i++)
		{
			string word = "";
			word += zy2ch_map[zy_index][3*i + 1];
			word += zy2ch_map[zy_index][3*i + 2];
			viterbi_map[i][count] = voc.getIndex(word.c_str());
		}
	}
}

double get_prob(const char *word1, const char *wor2, Vocab &voc, Ngram &lm)
{
	VocabIndex vi1;
	VocabIndex vi2;
	if(voc.getIndex(word1) == Vocab_None){
		vi1 = voc.getIndex(Vocab_Unknown);
	}
	else{
		vi1 = voc.getIndex(word1);
	}
	if(voc.getIndex(wor2) == Vocab_None){
		vi2 = voc.getIndex(Vocab_Unknown);
	}
	else{
		vi2 = voc.getIndex(wor2);
	}
	VocabIndex vi3[] = {vi1, Vocab_None};
	return lm.wordProb(vi2, vi3);
}

void get_viterbi_prob_and_viterbi_path(int zy_index, int count, char *pch, char *word, Vocab &voc, Ngram &lm)
{
	double prob;
	if(zy_index >= 0)
	{
		for(int i = 0; i < strlen(zy2ch_map[zy_index])/3; i++)
		{
			string tmp = "";
			tmp += zy2ch_map[zy_index][3*i + 1];
			tmp += zy2ch_map[zy_index][3*i + 2];
			if(count == 0)
			{
				viterbi_prob[i][count] = get_prob("<unk>", tmp.c_str(), voc, lm);
				viterbi_path[i][count] = -1;
			}
			else
			{
				double max_prob = -10000;
				int max_prob_index = -1;
				for(int j = 0; j < ch_num[count - 1]; j++)
				{
					if(viterbi_map[j][count - 1] == -1){
						prob =  get_prob("<unk>", tmp.c_str(), voc, lm) - 1000;
					}
					else{
						prob = get_prob(voc.getWord(viterbi_map[j][count - 1]), tmp.c_str(), voc, lm) + viterbi_prob[j][count - 1];
					}
					if(prob > max_prob)
					{
						max_prob = prob;
						max_prob_index = j;
					}
				}
				viterbi_prob[i][count] = max_prob;
				viterbi_path[i][count] = max_prob_index;
			}
		}
	}
	else
	{
		if(count == 0)
		{
			viterbi_prob[0][count] = get_prob("<unk>", pch, voc, lm);
			viterbi_path[0][count] = -1;
		}
		else
		{
			double max_prob = -10000;
			int max_prob_index = -1;
			for(int j = 0 ; j < ch_num[count - 1]; j++)
			{
				if(viterbi_map[j][count - 1] == -1){
					prob = get_prob("<unk>", pch, voc, lm) - 1000;
				}
				else{
					prob = get_prob(voc.getWord(viterbi_map[j][count - 1]), pch, voc, lm) + viterbi_prob[j][count - 1];
				}
				if(prob > max_prob)
				{
					max_prob = prob;
					max_prob_index = j;
				}
			}
			viterbi_prob[0][count] = max_prob;
			viterbi_path[0][count] = max_prob_index;
		}
	}
}

void print_out(int count, char *text, Vocab &voc)
{
	if(count == 0)
	{
		cout << "<s>" << text << "</s>" << endl;
	}
	else
	{
		double out_max = -10000;
		int out_max_index = -1;
		VocabIndex output[100];
		for(int i = 0; i < ch_num[count - 1]; i++)
		{
			if(viterbi_prob[i][count - 1] > out_max)
			{
				out_max = viterbi_prob[i][count - 1];
				out_max_index = i;
			}
		}
		for(int i = 0; i < count; i++)
		{
			output[count - 1 - i] = viterbi_map[out_max_index][count -1 - i];
			out_max_index = viterbi_path[out_max_index][count - 1 - i];
		}
		cout << "<s> ";
		for(int i = 0; i < count; i++)
		{
			cout << ((output[i] != -1) ? (voc.getWord(output[i])) : (viterbi_output[i])) << " ";
		}
		cout<<"</s>"<<endl;
	}
}

int main(int argc, char * argv[]){
	fstream test_data, map_data, zy_table_data;
	test_data.open(argv[1], ios::in);
	map_data.open(argv[2], ios::in);
	zy_table_data.open("zy_table.txt", ios::in);
	int order = atoi(argv[4]);

	Vocab voc;
 	Ngram lm(voc, order);
	{
		File lmFile(argv[3], "r");
		lm.read(lmFile);
		lmFile.close();
	}

	char text[800];
	char word[2];
	char *pch;
	int count;
	int zy_index;

	//Preprocessing
	for(int i = 0 ; i < 37; i++)
	{
		zy_table_data >> zy_table[i];
	}
	while(map_data >> word)
	{
		zy_index = Get_zy_index(word);
		if(zy_index == -1){
			map_data.getline(text, sizeof(text));
		}
		else{
			map_data.getline(zy2ch_map[zy_index], sizeof(zy2ch_map[zy_index]));
		}
	}

	//Main part
	while(test_data.getline(text, 800))
	{
		count = 0;

		pch = strtok(text, " ");
		while(pch != NULL)
		{
			zy_index = Repeated(pch);
			get_ch_num(zy_index, count);
			get_viterbi_map(zy_index, count, pch, voc);
			get_viterbi_prob_and_viterbi_path(zy_index, count, pch, word, voc, lm);
			strncpy(viterbi_output[count], word, 2);
			count++;
			pch = strtok(NULL, " ");
		}
		print_out(count, text, voc);
	}
	test_data.close();
	map_data.close();
	zy_table_data.close();

	return 0;
}
