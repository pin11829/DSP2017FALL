#include "hmm.h"
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60
using namespace std;

void printProgress (double percentage)
{
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
}

int main(int argc, char * argv[])
{
	//Input data
	char **data_train, buffer_char;
	char buffer_str[100];
	int state_n;
	int state_n_beta;
	int num = 0;
	int frame_total = 0;
	int sample_total = 0;
	int state_total = 6;
	double iteration_total = atoi(argv[1]);
	HMM hmm_initial;
	loadHMM(&hmm_initial, argv[2]);
	FILE *output_model = fopen(argv[4], "w");
	fstream data;
	//Compute the total number of the frames
	data.open(argv[3], ios::in);
	while(data.get(buffer_char))
	{
		if(buffer_char == '\n')
		{
			frame_total = num;
			break;
		}
		num ++;
	}
	data.close();

	//Cauculate the number of the samples
	data.open(argv[3], ios::in);
	while(!data.eof())
	{
		data.getline(buffer_str, sizeof(buffer_str));
		sample_total++;
	}
	sample_total--;
	data.close();

	data_train = new char *[sample_total];
	for(int i = 0; i < sample_total; i++)
	{
		data_train[i] = new char[frame_total + 1];
	}

	int data_row = 0;
	int data_column = 0;
	data.open(argv[3], ios::in);
	while(data.get(buffer_char))
	{
		if(buffer_char == '\n')
		{
			data_train[data_row][data_column] = 'n';
			data_row++;
			data_column = 0;
			continue;
		}
		data_train[data_row][data_column] = buffer_char;
		data_column++;
	}
	data.close();

	//initialize the parameter
	double alpha[frame_total][state_total];
	double beta[frame_total][state_total];
	double gmma[frame_total][state_total];
	double epsilon[frame_total][state_total][state_total];
	double gmma_initial_total[state_total];
	double gmma_sum_visit[state_total];
	double epsilon_sumfortrain[state_total][state_total];
	double gmma_all_state[state_total][state_total];
	double gmma_sum_visit_state[state_total];
	double gmma_sum = 0;
	double epsilon_sum = 0;
	for(int iteration = 0; iteration < iteration_total; iteration++)
	{
		printProgress((iteration/iteration_total));

		//Reset with iteration
		for(int i = 0; i < state_total; i++)
		{
			gmma_initial_total[i] = 0;
			gmma_sum_visit[i] = 0;
			gmma_sum_visit_state[i] = 0;
			for(int j = 0; j < state_total; j++)
			{
				gmma_all_state[i][j] = 0;
				epsilon_sumfortrain[i][j] = 0;
			}
		}

		//Cauculate alpha and beta
		for(int s = 0; s < sample_total; s++)
		{
			for(int i = 0; i < frame_total; i++)
			{
				if(i == 0)
				{
					state_n = data_train[s][0] - 'A';
					for(int j = 0; j < state_total; j++)
					{
						alpha[0][j] = (hmm_initial.initial[j])*(hmm_initial.observation[state_n][j]);
						beta[frame_total - 1][j] = 1;
					}
				}
				else
				{
					for(int j = 0; j < state_total; j++)
					{
						state_n = data_train[s][i] - 'A';
						state_n_beta = data_train[s][frame_total - i] - 'A';
						alpha[i][j] = 0;
						beta[frame_total - 1 - i][j] = 0;
						for(int k = 0; k < state_total; k++)
						{
							alpha[i][j] += ((alpha[i - 1][k])*hmm_initial.transition[k][j])*(hmm_initial.observation[state_n][j]);
							beta[frame_total - 1 - i][j] += (hmm_initial.transition[j][k])*(hmm_initial.observation[state_n_beta][k])*beta[frame_total - i][k];
						}
					}
				}
			}
			//Cauculate Gama
			gmma_sum = 0;
			for(int j = 0; j < state_total; j++)
			{
				gmma_sum += alpha[frame_total - 1][j];
			}
			for(int i = 0; i < frame_total; i++)
			{
				for(int j = 0; j < state_total; j++)
				{
					gmma[i][j] = (alpha[i][j]*beta[i][j])/gmma_sum;
				}
			}

			//Cauculate Epsilon
			for(int i = 0; i < frame_total - 1; i++)
			{
				state_n = data_train[s][i + 1] - 'A';
				for(int j = 0; j < state_total; j++)
				{
					for(int k = 0; k < state_total; k++)
					{
						epsilon[i][j][k] = ((alpha[i][j])*(hmm_initial.transition[j][k])*(hmm_initial.observation[state_n][k])*(beta[i + 1][k]))/gmma_sum;
					}
				}
			}

			//Cauculate the parameter for learning
			for(int i = 0; i < state_total; i++)
			{
				gmma_initial_total[i] += gmma[0][i];
				for(int j = 0; j < frame_total; j++)
				{
					gmma_sum_visit[i] += gmma[j][i];
					gmma_all_state[(data_train[s][j] - 'A')][i] += gmma[j][i];
					if(j != (frame_total - 1))
					{
						gmma_sum_visit_state[i] += gmma[j][i];
					}
				}
				for(int k = 0; k < state_total; k++)
				{
					for(int j = 0; j < frame_total - 1; j++)
					{
						epsilon_sumfortrain[i][k] += epsilon[j][i][k];
					}
				}
			}
		}
		//Cauculate the parameter
		for(int i = 0; i < state_total; i++)
		{
			hmm_initial.initial[i] = gmma_initial_total[i]/sample_total;
			for(int j = 0; j < state_total; j++)
			{
				hmm_initial.transition[i][j] = epsilon_sumfortrain[i][j]/gmma_sum_visit_state[i];
				hmm_initial.observation[j][i] = gmma_all_state[j][i]/gmma_sum_visit[i];
			}
		}
	}
	printf("%s\n", "");
	dumpHMM( output_model, &hmm_initial );
	fclose(output_model);
	return 0;
}
