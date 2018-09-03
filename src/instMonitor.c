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
#define ARITHMETIC_COUNTER 3
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

#define MATCH_SB 0x23 	 //Store byte
#define MATCH_SH 0x1023  //Store half word
#define MATCH_SW 0x2023  //Store word
#define MATCH_SD 0x3023  //Store double
#define MATCH_LB 0x3
#define MATCH_LH 0x1003
#define MATCH_LW 0x2003
#define MATCH_LD 0x3003
#define MATCH_LBU 0x4003
#define MATCH_LHU 0x5003
#define MATCH_LWU 0x6003
#define MATCH_BEQ 0x63
#define MATCH_BNE 0x1063
#define MATCH_BLT 0x4063
#define MATCH_BGE 0x5063
#define MATCH_BLTU 0x6063
#define MATCH_BGEU 0x7063
#define MATCH_JALR 0x67
#define MATCH_JAL 0x6f
#define MATCH_ADDI 0x13 //Mask is same of SW,LW,etc..
#define MATCH_SLLI 0x1013
#define MATCH_SLTI 0x2013
#define MATCH_SLTIU 0x3013
#define MATCH_XORI 0x4013
#define MATCH_SRLI 0x5013
#define MATCH_SRAI 0x40005013
#define MATCH_ORI 0x6013
#define MATCH_ANDI 0x7013
#define MATCH_ADD 0x33
#define MATCH_SUB 0x40000033
#define MATCH_SLL 0x1033
#define MATCH_SLT 0x2033
#define MATCH_SLTU 0x3033
#define MATCH_XOR 0x4033
#define MATCH_SRL 0x5033
#define MATCH_SRA 0x40005033
#define MATCH_OR 0x6033
#define MATCH_AND 0x7033
#define MATCH_ADDIW 0x1b
#define MATCH_SLLIW 0x101b
#define MATCH_SRLIW 0x501b
#define MATCH_SRAIW 0x4000501b
#define MATCH_ADDW 0x3b
#define MATCH_SUBW 0x4000003b
#define MATCH_SRLW 0x503b
#define MATCH_SRAW 0x4000503b

//Masks
#define MASK_ARRAY_LENGTH 5
#define MASK_JAL 0x7f
#define MASK_ADD  0xfe00707f
#define MASK_SLLI  0xfc00707f //SRLI,SRAI have the same mask.
#define MASK_STORE_LOAD_BRANCH 0x707f  //Stores, loads and branches  have the same mask



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
  unsigned int maskArray[MASK_ARRAY_LENGTH] = {MASK_STORE_LOAD_BRANCH,
                                              MASK_JAL,
                                              MASK_SLLI,
                                              MASK_JAL,
                                              MASK_ADD};
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
        // long int counterline = 0;

        int i;

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
            // counterline++;


            //printf("\nInstruction hex:%x\n",instruction);
            int maskFound = 0;

            //Each instruction can only have one mask match at a time.
              for(i = 0; i < MASK_ARRAY_LENGTH ; i++){
                //Perfoming an AND operation to match the instruction.
                if(!maskFound){
                  match = instruction & maskArray[i];
                  //See what instruction match we got.
                  // printf("\nInstruction hex:%x\n",instruction);
                  // printf("\nMask analized:%x\n",maskArray[i]);
                  // printf("\nMatch result%x\n",match);
                  switch (match) {
                    case MATCH_SB:
                    case MATCH_SD:
                    case MATCH_SH:
                    case MATCH_SW:{
                      maskFound = 1;
                      counter[STORE_COUNTER]++;
                    }
                    break;
                    case MATCH_LB:
                    case MATCH_LD:
                    case MATCH_LH:
                    case MATCH_LW:
                    case MATCH_LBU:
                    case MATCH_LHU:
                    case MATCH_LWU:{
                      maskFound = 1;
                      counter[LOAD_COUNTER]++;
                    }
                    break;
                    case MATCH_BEQ:
                    case MATCH_BNE:
                    case MATCH_BLT:
                    case MATCH_BGE:
                    case MATCH_BLTU:
                    case MATCH_BGEU:
                    case MATCH_JALR:
                    case MATCH_JAL:{
                      maskFound = 1;
                      counter[BRANCH_COUNTER]++;
                    }
                    break;
                    case MATCH_ADDI:
                    case MATCH_SLLI:
                    case MATCH_SLTI:
                    case MATCH_SLTIU:
                    case MATCH_XORI:
                    case MATCH_SRLI:
                    case MATCH_SRAI:
                    case MATCH_ORI:
                    case MATCH_ANDI:
                    case MATCH_ADD:
                    case MATCH_SUB:
                    case MATCH_SLL:
                    case MATCH_SLT:
                    case MATCH_SLTU:
                    case MATCH_XOR:
                    case MATCH_SRL:
                    case MATCH_SRA:
                    case MATCH_OR:
                    case MATCH_AND:
                    case MATCH_ADDIW:
                    case MATCH_SLLIW:
                    case MATCH_SRLIW:
                    case MATCH_SRAIW:
                    case MATCH_ADDW:
                    case MATCH_SUBW:
                    case MATCH_SRLW:
                    case MATCH_SRAW:{
                      maskFound = 1;
                      counter[ARITHMETIC_COUNTER]++;
                    }break;
                    default:{

                    }break;
                  }
                }
                // printf("Mask found:%d\n",maskFound);
                // getchar();
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
        printf("\tSTORE:\t\t%d\n",counter[STORE_COUNTER]);
        printf("\tLOAD:\t\t%d\n",counter[LOAD_COUNTER]);
        printf("\tBRANCHES:\t%d\n",counter[BRANCH_COUNTER]);
        printf("\tARITHMETIC:\t%d\n",counter[ARITHMETIC_COUNTER]);
        printf("\\--------------------------------------\\\n");
        // printf("lines from main:%ld\n",counterline);
      }
    }
  }
  return 0;
}
