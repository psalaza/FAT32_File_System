#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {


	char command[100];
	char exit[100];
	strcpy(exit, "exit");
	unsigned char bpb[2];
	unsigned char bpb1[2];
	unsigned char bpb2[2];
	unsigned char bpb3[2];
	unsigned char bpb4[4];
	unsigned char bpb5[4];
	unsigned char bpb6[4];
	unsigned char name[1000];
	unsigned char fullname[1000];


	unsigned int firstSector;
	//read in user input... all commands available have been implemented.
	unsigned int fileNumber;
	unsigned int fileSec;
	unsigned int capture;
	fileNumber = open(argv[1], O_RDWR);
	lseek(fileNumber, 11, SEEK_SET);
	read(fileNumber, bpb, 2);
	lseek(fileNumber, 13, SEEK_SET);
	read(fileNumber, bpb1, 1);
	lseek(fileNumber, 14, SEEK_SET);
	read(fileNumber, bpb2, 2);
	lseek(fileNumber, 16, SEEK_SET);
	read(fileNumber, bpb3, 1);
	lseek(fileNumber, 32, SEEK_SET);
	read(fileNumber, bpb4, 4);
	lseek(fileNumber, 36, SEEK_SET);
	read(fileNumber, bpb5, 4);
	lseek(fileNumber, 44, SEEK_SET);
	read(fileNumber, bpb6, 4);
	unsigned int capture4 = *(int*)bpb1;
	unsigned int capture3 = *(int*)bpb;
	unsigned int capture2 = *(int*)bpb6;
	unsigned int rsv = *(int*)bpb2;
	unsigned int nFats = *(int*)bpb3;
	unsigned int fSize = *(int*)bpb5;
	//int rootDir;
		//printf("Sector per Cluser: %d\n", capture);
	fileSec = rsv + (nFats*fSize);
	firstSector = ((capture2 - 2)*capture4) + fileSec;
	unsigned  int offset = firstSector * capture3;
	unsigned  int offsetCluster = (rsv * capture3) + (capture2 * 4);
	while (strcmp(command, "exit") != 0) {
		printf("$ ");
		scanf("%s", command);

		if (strcmp(command, "info") == 0) {

			capture = *(int*)bpb;
			printf("Bytes per sec: %d\n", capture);

			capture = *(int*)bpb1;
			printf("Sector per Cluser: %d\n", capture);

			capture = *(int*)bpb2;
			printf("reseverd sector count: %d\n", capture);

			capture = *(int*)bpb3;
			printf("number of FATs: %d\n", capture);

			capture = *(int*)bpb4;
			printf("total sectors %d\n", capture);

			capture = *(int*)bpb5;
			printf("FATsize: %d\n", capture);

			capture = *(int*)bpb6;
			printf("FAT Root Cluster: %d\n", capture);
			//printf("%d", *i);

		}
		else if (strcmp(command, "ls") == 0) {
			lseek(fileNumber, offset, SEEK_SET);
			read(fileNumber, name, 4);
			printf("%d\n", fileSec);
			printf("%d\n", firstSector);
			printf("%d\n", offset);
			printf("%s\n", name);
			sprintf(fullname, "%s", name);
			printf("%s\n", fullname);
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
	memset(bpb, 0, sizeof(bpb));
	memset(bpb1, 0, sizeof(bpb1));
	memset(bpb2, 0, sizeof(bpb2));
	memset(bpb3, 0, sizeof(bpb3));
	memset(bpb4, 0, sizeof(bpb4));
	memset(bpb5, 0, sizeof(bpb5));
	memset(bpb6, 0, sizeof(bpb6));
	return 0;
}