//INCLUDES
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//DEFINES AND CONSTATS
#define MAX_FILES 3
#define BUFFER_SIZE 100
#define MAIN_IDENTIFIER "<main>:"


//AUX FUNCTIONS





//MAIN PROGRAM
int main(int argc, char const *argv[]) {

  char commitBuffer[BUFFER_SIZE];
  char bufferPC[BUFFER_SIZE];
  char bufferMain[BUFFER_SIZE];
  char dumpMain[BUFFER_SIZE];
  char mainPC[BUFFER_SIZE] = "0x00000000";
  int stringLenght;

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

            /*Concatenates both strings, formating the PC to use it on the commit file.
              0x00000000 + 80001a64: ------> 0x0000000080001a64
            */
            strcat(mainPC,dumpMain);
            printf("\nPC DA MAIN:%s\n",mainPC);
            fclose(dumpFile);
          }
        }
        //Open commit file and count instructions commited
        printf("debug\n");

        //Reads and discard the first line
        fgets(commitBuffer,BUFFER_SIZE,commitFile);

        while(!feof(commitFile)){
          fgets(commitBuffer,BUFFER_SIZE,commitFile);
          puts(commitBuffer);
          getchar();
        }

      }
    }
  }







  return 0;
}
