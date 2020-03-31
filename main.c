#include <stdio.h>
#include <string.h>


int main(){


        char command[100];
        char exit[100];
        strcpy(exit,"exit");

	//read in user input... all commands available have been implemented.

        while(strcmp(command,"exit") != 0){
                printf("$ ");
                scanf("%s",command);

                if(strcmp(command,"info") == 0){

                        printf("info entered\n");
                }
                else if(strcmp(command,"ls") == 0){

                        printf("ls entered\n");
                }
                else if(strcmp(command,"cd") == 0){

                        printf("cd entered\n");
                }
                else if(strcmp(command,"creat") == 0){

                        printf("creat entered\n");
                }
                else if(strcmp(command,"mkdir") == 0){

                        printf("mkdir entered\n");
                }
                else if(strcmp(command,"mv") == 0){

                        printf("mv entered\n");
                }
                else if(strcmp(command,"open") == 0){

                        printf("open entered\n");
                }
                else if(strcmp(command,"close") == 0){

                        printf("close entered\n");
                }
                else if(strcmp(command,"read") == 0){

                        printf("read entered\n");
                }
                else if(strcmp(command,"write") == 0){

                        printf("write entered\n");
                }
                else if(strcmp(command,"exit") == 0)
                        printf("exiting program. Good Bye\n");
                else
                        printf("invalid command, try again...\n");


        }

        //release all allocated resources here

        return 0;
}

