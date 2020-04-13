  
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

	char command[100];
	char exit[100];
	strcpy(exit, "exit");
	unsigned char bpb[100];
	unsigned char bpb1[100];
	unsigned char bpb2[100];
	unsigned char bpb3[100];
	unsigned char bpb4[100];
	unsigned char bpb5[100];
	unsigned char bpb6[100];



	//read in user input... all commands available have been implemented.
	int fileNumber;
	int capture;
	fileNumber = open(argv[1], O_RDWR);
	while (strcmp(command, "exit") != 0) {
		printf("$ ");
		char t = '\0';
		scanf("%s", command);
		t = getchar();
		if (strcmp(command, "info") == 0) {
			lseek(fileNumber, 11, SEEK_SET);
			read(fileNumber, bpb, 2);
			capture = *(int*)bpb;
			printf("Bytes per sec: %d\n", capture);
			lseek(fileNumber, 13, SEEK_SET);

			read(fileNumber, bpb1, 1);
			capture = *(int*)bpb1;
			printf("Sector per Cluser: %d\n", capture);
			lseek(fileNumber, 14, SEEK_SET);
			read(fileNumber, bpb2, 2);
			capture = *(int*)bpb2;
			printf("reseverd sector count: %d\n", capture);
			lseek(fileNumber, 16, SEEK_SET);
			read(fileNumber, bpb3, 1);
			capture = *(int*)bpb3;
			printf("number of FATs: %d\n", capture);
			lseek(fileNumber, 32, SEEK_SET);
			read(fileNumber, bpb4, 4);
			capture = *(int*)bpb4;
			printf("total sectors %d\n", capture);
			lseek(fileNumber, 36, SEEK_SET);
			read(fileNumber, bpb5, 4);
			capture = *(int*)bpb5;
			printf("FATsize: %d\n", capture);
			lseek(fileNumber, 44, SEEK_SET);
			read(fileNumber, bpb6, 4);
			capture = *(int*)bpb6;
			printf("FAT Root Cluster: %d\n", capture);
			//printf("%d", *i);

		}
		else if (strcmp(command, "ls") == 0) {
			// c variable is the variable that will hold the second argument when ls is being called 
			char c[100];
			
			if(t == ' '){
				scanf("%s",c);
				printf("second command: %s\n",c);
			}
			else
				printf("nothing entered with ls\n");


		}
		else if (strcmp(command, "cd") == 0) {

			printf("cd entered\n");
		}
		else if (strcmp(command, "creat") == 0) {

			printf("creat entered\n");
		}
		else if (strcmp(command, "mkdir") == 0) {

			printf("mkdir entered\n");
		}
		else if (strcmp(command, "mv") == 0) {

			printf("mv entered\n");
		}
		else if (strcmp(command, "open") == 0) {

			printf("open entered\n");
		}
		else if (strcmp(command, "close") == 0) {

			printf("close entered\n");
		}
		else if (strcmp(command, "read") == 0) {

			printf("read entered\n");
		}
		else if (strcmp(command, "write") == 0) {

			printf("write entered\n");
		}
		else if (strcmp(command, "exit") == 0)
			printf("exiting program. Good Bye\n");
		else
			printf("invalid command, try again...\n");


	}
	close(fileNumber);
	//release all allocated resources here

	return 0;
}
