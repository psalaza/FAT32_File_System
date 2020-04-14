#include <stdio.h>
#include <string.h>
#include <fcntl.h>
struct DirectoryEntry {
	unsigned char DIR_Name[11];
	unsigned char DIR_Attr;
	unsigned char DIR_NTRes;
	unsigned char DIR_CrtTimeTenth;
	unsigned short DIR_CrtTime;
	unsigned short DIR_CrtDate;
	unsigned short DIR_LstAccDate;
	unsigned short DIR_FstClusHI;
	unsigned short DIR_WrtTime;
	unsigned short DIR_WrtDate;
	unsigned short DIR_FstClusLO;
	unsigned int DIR_FileSize;
} __attribute((packed));
int clusterFromPath(char * name);
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

	//clusterFromPath("./GREEN");
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
	unsigned  int offsetCluster;
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
			int count = 0;
			int count2 = 0;
			int offset2 = 0;
			struct DirectoryEntry direct;
			unsigned char *one;
			while (1)
			{


				offsetCluster = (rsv * capture3) + (capture2 * 4);
				lseek(fileNumber, offset + (count * 32), SEEK_SET);
				read(fileNumber, name, 32);
				memcpy(&direct, name, sizeof(struct DirectoryEntry));
				one = strchr(direct.DIR_Name, ' ');
				if (one != NULL && 11 > (one - direct.DIR_Name)) {
					direct.DIR_Name[(one - direct.DIR_Name)] = '\0';
				}

				if (direct.DIR_Name[0] == 0x0) {

					break;
				}
				else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
					count++;
				}
				else if (strcmp(direct.DIR_Name, "GREEN") == 0) {
					//		printf("%s\n", direct.DIR_Name);
					//		printf("%d\n", direct.DIR_FstClusHI);
					//		printf("%d\n", direct.DIR_FstClusLO);
					offset2 = (((direct.DIR_FstClusLO - 2)*capture4) + fileSec)*capture3;
					while (1)
					{


						//printf("%d\n", offset2);
						lseek(fileNumber, offset2 + (count2 * 32), SEEK_SET);
						read(fileNumber, name, 32);
						memcpy(&direct, name, sizeof(struct DirectoryEntry));
						one = strchr(direct.DIR_Name, ' ');
						if (one != NULL && 11 > (one - direct.DIR_Name)) {
							direct.DIR_Name[(one - direct.DIR_Name)] = '\0';
						}

						if (direct.DIR_Name[0] == 0x0) {

							break;
						}
						else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
							count2++;
						}
						else {
							printf("%s\n", direct.DIR_Name);
							count2++;
						}
					}
					break;
				}
				else {
					count++;
				}

			}
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
		else if (strcmp(command, "size") == 0) {

			int count = 0;
			struct DirectoryEntry direct;
			unsigned char *one1;
			while (1)
			{


				offsetCluster = (rsv * capture3) + (capture2 * 4);
				lseek(fileNumber, offset + (count * 32), SEEK_SET);
				read(fileNumber, name, 32);
				memcpy(&direct, name, sizeof(struct DirectoryEntry));
				one1 = strchr(direct.DIR_Name, ' ');
				if (one1 != NULL && 11 > (one1 - direct.DIR_Name)) {
					direct.DIR_Name[(one1 - direct.DIR_Name)] = '\0';
				}

				if (direct.DIR_Name[0] == 0x0) {

					break;
				}

				else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
					count++;
				}
				else if (strcmp(direct.DIR_Name, "GREEN") == 0) {
					printf("%d\n", direct.DIR_FileSize);
					break;
				}

				else {
					count++;
				}

			}
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
int clusterFromPath(char * name) {
	int i;
	int count = 0;
	struct DirectoryEntry direct;
	int fullString = 1;
	int catch22 = 0;
	int begining = 0;
	char *ptr;
	char *file;
	char *finisher;
	char **incompletePath;
	char *holder = (char*)malloc((strlen(name)) * sizeof(char));

	for (i = 0; i < strlen(name) + 1; i++) {

		if (name[i] == '/' || name[i] == '\0') {

			i++;
			memcpy(holder, name + begining, i - begining);
			holder[i - (begining + 1)] = '\0';

			if (catch22 == 0) {

				incompletePath = (char**)malloc(sizeof(char*));
			}
			else {

				incompletePath = (char**)realloc(incompletePath, (catch22 + 1) * sizeof(char*));
			}


			incompletePath[catch22] = (char *)malloc((strlen(holder) + 2) * sizeof(char));
			strcpy(incompletePath[catch22], holder);

			printf("%s\n", incompletePath[catch22]);
			printf("%d\n", catch22++);
			begining = i;
			if (name[i - 1] == '\0') {
				catch22--;
				break;
			}
		}



	}
	/*printf("%d\n", catch22);
	for (i = 0; i <= catch22; i++) {
		printf("%s\n", incompletePath[i]);
		while (1)
		{


			//offsetCluster = (rsv * capture3) + (capture2 * 4);
			lseek(fileNumber, offset + (count * 32), SEEK_SET);
			read(fileNumber, name, 32);
			memcpy(&direct, name, sizeof(struct DirectoryEntry));

			if (direct.DIR_Name[0] == 0x0) {

				return -1;
			}
			else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
				count++;
			}
				else if(strcmp(direct.DIR_Name, incompletePath[i])==0){
					printf("%d\n", incompletePath[i]);
				break;
				}

		}
	}
	*/

	return 0;
}