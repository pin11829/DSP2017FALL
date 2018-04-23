#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

char zy2ch_map[37][2000*3][3];
int zy2ch_count[37] = {0};//The number of the words in the ZY
char zy_table[37][2];
char unq_ch[37][2];
char all_ch[20000][2];

//Check whether the character is repeated or not
int repeated(int count, char *pch)
{
  for(int i = 0; i < count; i++){
    if(strncmp(unq_ch[i], pch, 2) == 0){
      return 1;
    }
  }
  return 0;
}

//Return the index in the zy_table
int get_zy_index(char *pch)
{
  for(int i = 0; i < 37; i++){
    if(strncmp(zy_table[i], pch, 2) == 0){
      cout<<i<<"\n";
      return i;
    }
  }
  return -1;
}

int main(int argc, char * argv[]){
  char word[2];//One word needs two spaces
  char buffer[2];//Use buffer to check
  char zy_part[70];//put ZY
  char *pch;//Use pch to collect word
  int zy_index;//the ZY's index in the table
  int count;//Read the count-nd number
  int ch_num = 0;

  fstream ch2zy_file, zy2ch_file, zy_table_file;
  ch2zy_file.open(argv[1], ios::in);
  zy2ch_file.open(argv[2], ios::out);
  zy_table_file.open("zy_table.txt", ios::in);

  //Fill the table with 37 ZYs
  for(int i = 0; i < 37; i++){
    zy_table_file >> zy_table[i];
  }

  while(!ch2zy_file.eof()){
    ch2zy_file >> buffer;//because if using the eof, it could cause the repeat loop and need to check
    ch2zy_file >> zy_part;
    if(strncmp(buffer, word, 2) == 0){
      break;
    }
    strncpy(word, buffer, 2);
		strncpy(all_ch[ch_num], word, 2);//Collect all the Chinese words

    count = 0;
    pch = strtok(zy_part, "/");
    while(pch != NULL){
      //Check wether this ZY's line have pch or not
      if(!repeated(count, pch)){
        //Find out which zy it is
        zy_index = get_zy_index(pch);

        //Fill the table
        strncpy(zy2ch_map[zy_index][zy2ch_count[zy_index]], word, 2);
        strncpy(unq_ch[count], pch, 2);
        zy2ch_count[zy_index] = zy2ch_count[zy_index] + 2;
        count++;
      }
      pch = strtok(NULL, "/");
    }
    ch_num++;
  }

  //Output zy --> Chinese
  for(int i = 0; i < 37; i++){
    strncpy(word, zy_table[i], 2);
    zy2ch_file << word << "\t";
    for(int j = 0; j < zy2ch_count[i]; j++){
      zy2ch_file << zy2ch_map[i][j] << " ";
    }
    zy2ch_file << "\n";
  }

  //Output Chinese to Chinese
  for(int i = 0; i < ch_num; i++){
    strncpy(word, all_ch[i], 2);
    zy2ch_file << word << "\t" << word << "\n";
  }

  ch2zy_file.close();
  zy2ch_file.close();
  zy_table_file.close();
  return 0;
}
