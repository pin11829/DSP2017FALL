#include "hmm.h"
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

int main(int argc, char * argv[])
{
	//Input data
	char buffer_char;
	char data_train[100];
	int state_n;
	int num = 0;
	int frame_total = 0;
	int sample_total = 0;
	int state_total = 6;
	int model_total = 5;
	char modelforacc[5] = {'1', '2', '3', '4', '5'};
	fstream data, testing_answer, output_result, output_accuracy;
	data.open(argv[2], ios::in);

	//Cauculate the number of frames
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

	//Cauculate Viterbi
	double delta[frame_total][state_total];
	double probabilityofpath[model_total];
	double maxprobability = 0;
	double deltaselect = 0;
	int model_max;
	double correct_count = 0;
	char answer[100];

	HMM hmm_initial[model_total];
	load_models(argv[1], hmm_initial, 5);
	output_result.open(argv[3],ios::out|ios::trunc);
	output_accuracy.open("acc.txt",ios::out|ios::trunc);
	testing_answer.open("testing_answer.txt",ios::in);
	data.open(argv[2], ios::in);
	while(!data.eof())
	{
		data.getline(data_train, sizeof(data_train));
		testing_answer.getline(answer, 100);
		if(data.eof())
		{
			break;
		}
		//Through all the model
		for(int m = 0; m < model_total; m++)
		{
			//Reset the delta
			for(int i = 0; i < frame_total; i++)
			{
				for(int j = 0; j < state_total; j++)
				{
					delta[i][j] = 0;
				}
			}

			for(int i = 0; i < frame_total; i++)
			{
				state_n = data_train[i] - 'A';
				if(i == 0)
				{
					for(int j = 0; j < state_total; j++)
					{
						delta[0][j] = (hmm_initial[m].initial[j])*(hmm_initial[m].observation[state_n][j]);
					}
				}
				else
				{
					for(int j = 0; j < state_total; j++)
					{
						for(int k = 0; k < state_total; k++)
						{
							deltaselect = delta[i - 1][k]*hmm_initial[m].transition[k][j]*hmm_initial[m].observation[state_n][j];
							if(deltaselect > delta[i][j])
							{
								delta[i][j] = deltaselect;
							}
						}
					}
				}
			}

			//Find the maximum probability of the path in the model
			probabilityofpath[m] = 0;
			for(int i = 0; i < state_total; i++)
			{
				if(delta[frame_total - 1][i] > probabilityofpath[m])
				{
					probabilityofpath[m] = delta[frame_total - 1][i];
				}
			}
		}

		//Find the max model
		maxprobability = 0;
		for(int m = 0; m < model_total; m++)
		{
			if(maxprobability < probabilityofpath[m])
			{
				maxprobability = probabilityofpath[m];
				model_max = m;
			}
		}

		//Output result
		output_result << "model_0" << (model_max + 1) << ".txt\t" << maxprobability << "\n";

		//Cauculate the accuracy
		sample_total++;
		if(answer[7] == modelforacc[model_max])
		{
			correct_count++;
		}
	}
	sample_total--;

	//Output accuracy
	output_accuracy << (correct_count/sample_total);
	printf("%f\n", correct_count/sample_total);
	data.close();
	testing_answer.close();
	output_accuracy.close();
	output_result.close();

	return 0;
}
