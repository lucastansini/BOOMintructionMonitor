//INCLUDES
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//DEFINES AND CONSTATS
#define MAX_FILES 3
#define BUFFER_SIZE 100
#define MAIN_IDENTIFIER "<main>:"




//Index of the counter array
#define STORE_COUNTER 0
#define LOAD_COUNTER 1
#define BRANCH_COUNTER 2
//.
//.
//Add more counters here
//.
//.
#define COUNTER_ARRAY_LENGTH 30 //We can define until 30 types to count
//TODO define a more modular way of storing the counters

//MASKS,MATCHES, AND INSTRUCTIONS
#define INSTRUCTION_RET 0x00008067

//Change the lenght of the array when you add more masks.
//TODO maybe making this modular
#define MASK_ARRAY_LENGTH 4
#define MATCH_SB 0x23 	 //Store byte
#define MASK_SB  0x707f
#define MATCH_SH 0x1023  //Store half word
#define MASK_SH  0x707f
#define MATCH_SW 0x2023  //Store word
#define MASK_SW  0x707f
#define MATCH_SD 0x3023  //Store double
#define MASK_SD  0x707f




//MAIN PROGRAM
int main(int argc, char const *argv[]) {

  char commitBuffer[BUFFER_SIZE];
  char bufferPC[BUFFER_SIZE];
  char bufferMain[BUFFER_SIZE];
  char dumpMain[BUFFER_SIZE];
  char mainPC[BUFFER_SIZE];
  int stringLenght = 0;
  int beginCount = 0;

  /*
    Buffer variables for the commit file
    Commit file has this format:
    3 0x0000000000010048 (0x03c58593) x11 0x0000000000010080
    3 0x0000000000010048 (0x03c58593) x 8 0x0000000000010080
  */
  int first;
  unsigned int programCounter;
  unsigned int instruction;

  /*Array of instruction masks*/
  unsigned int maskArray[MASK_ARRAY_LENGTH] = {MASK_SB,MASK_SD,MASK_SH,MASK_SW};
  //Variable that stores the result of the AND operation between instruction and mask.
  unsigned int match;

  /*Counter array recieving the indexes defined above*/
  int counter[COUNTER_ARRAY_LENGTH] = {};


  FILE *dumpFile;
  FILE *commitFile;

  if(argc != MAX_FILES){
    printf("Please provide the dump and commit file as arguments.\n");
    printf("Expecting: <dump_file> <commit_file>\n");

    return -1;

  }else{
    if(!(dumpFile = fopen(argv[1],"r"))){
      printf("Error opening the dump file.\n");
      return -2;
    }else{
      if(!(commitFile = fopen(argv[2],"r"))){
        printf("Error opening the commit file.\n");
        return -3;
      }else{
        //Find the PC where main starts.
        while(!feof(dumpFile)){
          //Read untill main
          fscanf(dumpFile,"%s",bufferMain);

          if(!strcmp(bufferMain,MAIN_IDENTIFIER)){
            //Read main PC
            fscanf(dumpFile,"%s",bufferPC);
            strcpy(dumpMain,bufferPC);

            stringLenght = strlen(dumpMain);
            dumpMain[stringLenght - 1] = ' ';

            strcpy(mainPC,dumpMain);

            fclose(dumpFile);
          }
        }

        //Open commit file and count instructions commited
        //Reads and discard the first line
        fgets(commitBuffer,BUFFER_SIZE,commitFile);

        /*
          Read and counts the instructions on the format of the commit file
          Example of the expected format
          3 0x0000000000010048 (0x03c58593) x11 0x0000000000010080
          3 0x0000000000010048 (0x03c58593) x 8 0x0000000000010080
        */
        //Convert the string to integer
        unsigned int mainPCnumber = (int)strtol(mainPC,NULL,16);

        while(!feof(commitFile)){
          fscanf(commitFile,"%d %x (%x)",&first,&programCounter,&instruction);

          //Read untill the end of the line
          fgets(commitBuffer,BUFFER_SIZE,commitFile);

          //Read untill we find the main piece executed.
          if(programCounter == mainPCnumber){
            //Begin processing the file
            beginCount = 1;
          }

          if(beginCount){

            int i;

            for(i = 0; i < MASK_ARRAY_LENGTH ; i++){
              //Perfoming an AND operation to match the instruction.
              match = instruction & maskArray[i];
              //See what instruction match we got.
              switch (match) {
                case MATCH_SW:{
                  counter[STORE_COUNTER]++;
                }
                break;
              }
            }
          }
          //We went untill the first RET instruction inside main.
          if((instruction == INSTRUCTION_RET)){
            beginCount = 0;
          }
        }
        //TODO maybe treat the vector as an object ? kinda nice
        //Show the results
        printf("\\--------------------------------------\\\n");
        printf("\tSTORE: %d\n",counter[STORE_COUNTER]);
        printf("\\--------------------------------------\\\n");
      }
    }
  }
  return 0;
}
