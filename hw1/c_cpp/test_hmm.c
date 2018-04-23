#include "hmm.h"
#include <math.h>

int main()
{
	char buffer_char
	FILE *outputfile = "model_01.txt";
	HMM hmm_initial;
	loadHMM( &hmm_initial, "model_init.txt" );
	fstream data;
	data.open_or_die(seq_model_01.txt, "r");
	while(input_data.get(buffer_char))
	{
		if(buffer_char == '\n')
		{
			frame_total = megumi;
			break;
		}
		megumi ++;
	}
	input_data.close();

	input_data.open(seq_model_01.txt, "r");
	while(!input_data.eof())
	{
		input_data.getline(buffer_str, sizeof(buffer_str));
		sample_total++;
	}
	sample_total--;
	input_data.close();



/*
	HMM hmms[5];
	load_models( "modellist.txt", hmms, 5);
	dump_models( hmms, 5);
*/
/*
	HMM hmm_initial;
	loadHMM( &hmm_initial, "model_init.txt" );
	dumpHMM( stderr, &hmm_initial );
*/


	printf("%f\n", log(1.5) ); // make sure the math library is included
	return 0;
}
