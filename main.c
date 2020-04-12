#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {


	char command[100];
	char exit[100];
	strcpy(exit, "exit");
	unsigned char buffer[100];



	//read in user input... all commands available have been implemented.
	int fileNumber;
	int capture;
	fileNumber = open(argv[1], O_RDWR);
	while (strcmp(command, "exit") != 0) {
		printf("$ ");
		scanf("%s", command);

		if (strcmp(command, "info") == 0) {
			lseek(fileNumber, 11, SEEK_SET);
			read(fileNumber, buffer, 2);
			capture = *(int*)buffer;
			printf("Bytes per sec: %d\n", capture);
			lseek(fileNumber, 13, SEEK_SET);
			read(fileNumber, buffer, 1);
			capture = *(int*)buffer;
			printf("Sector per Cluser: %d\n", capture);
			lseek(fileNumber, 14, SEEK_SET);
			read(fileNumber, buffer, 2);
			capture = *(int*)buffer;
			printf("reseverd sector count: %d\n", capture);
			lseek(fileNumber, 16, SEEK_SET);
			read(fileNumber, buffer, 1);
			capture = *(int*)buffer;
			printf("number of FATs: %d\n", capture);
			lseek(fileNumber, 32, SEEK_SET);
			read(fileNumber, buffer, 4);
			capture = *(int*)buffer;
			printf("total sectors %d\n", capture);
			lseek(fileNumber, 36, SEEK_SET);
			read(fileNumber, buffer, 4);
			capture = *(int*)buffer;
			printf("FATsize: %d\n", capture);
			lseek(fileNumber, 44, SEEK_SET);
			read(fileNumber, buffer, 4);
			capture = *(int*)buffer;
			printf("FAT Root Cluster: %d\n", capture);
			//printf("%d", *i);

		}
		else if (strcmp(command, "ls") == 0) {

			printf("ls entered\n");
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
