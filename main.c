#include <stdio.h>
#include <string.h>
#include <ctype.h>
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

typedef struct file {
	unsigned int size;
	unsigned int mode;
	char name[20];
} file;

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

	int rootDir = 0;
	//printf("Sector per Cluser: %d\n", capture);

	fileSec = rsv + (nFats*fSize);
	firstSector = ((capture2 - 2)*capture4) + fileSec;
	unsigned  int offset = firstSector * capture3;
	unsigned  int offsetCluster;
	while (strcmp(command, "exit") != 0) {
		char cmd2[100];
		printf("$ ");
		char temp = '\0';
		scanf("%s", command);
		temp = getchar();

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
			int count3 = 0;
			int total = 0;
			int offset2 = 0;
			char keep[4];
			char keep2[4];
			struct DirectoryEntry direct;
			unsigned char *one;
			unsigned int newCap = capture2;
			unsigned int newCap2 = capture2;

			if (temp == ' ') {
				scanf("%s", cmd2);
			}
			/*	if (strcmp("", cmd2) == 0) {
					printf("%s\n", "File Does Not exist");
					memset(cmd2, 0, sizeof(cmd2));
				}*/
				//printf("%s\n", cmd2);

			//	break;
			while (1)
			{
				//	printf("%s", "HELLO1");
				offsetCluster = (512 * 32) + ((newCap2 * 4));
				offset2 = (((newCap2 - 2)*capture4) + fileSec)*capture3;
				lseek(fileNumber, offsetCluster, SEEK_SET);
				read(fileNumber, keep2, 4);
				newCap = *(int*)keep2;
				count = 0;
				//	printf("stuck");
				while (1)
				{
					//	printf("%s", "HELLO2");
					//	printf("stuck2");

					//	offsetCluster = (512 * 32) + (capture2 * 4);
					lseek(fileNumber, offset + (count * 32), SEEK_SET);
					read(fileNumber, name, 32);
					memcpy(&direct, name, sizeof(struct DirectoryEntry));
					//	printf("%s\n", direct.DIR_Name);
					one = strchr(direct.DIR_Name, ' ');
					if (one != NULL && 11 > (one - direct.DIR_Name)) {
						direct.DIR_Name[(one - direct.DIR_Name)] = '\0';

					}

					if (direct.DIR_Name[0] == 0x0) {
						//	printf("%s", "HELLO4");
						printf("%s\n", "Directory doesnt exist");
						count3++;
						break;
					}


					else if (direct.DIR_Attr == 0x20 && strcmp(direct.DIR_Name, cmd2) == 0) {
						//	printf("%s", "HELLO4");
						printf("%s\n", "This is a file");
						count3++;
						break;
					}
					else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
						//	printf("%s", "HELLO5");
						count++;
					}
					else if (strcmp(direct.DIR_Name, cmd2) == 0 || strcmp("", cmd2) == 0 || ((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
						//printf("%s\n",cmd2);

						//		printf("%d\n", direct.DIR_FstClusHI);
						//		printf("%d\n", direct.DIR_FstClusLO);


								//break;
					//	printf("%s", "HELLO5");
						if (strcmp("", cmd2) == 0) {
							//	printf("%s", "HELLO4");
							newCap = capture2;
						}
						else if (((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
							newCap = *(int*)bpb6;
							//	printf("%s", "HELLO3");
						}
						else
						{
							//	printf("%s", "HELLO4");
							newCap = direct.DIR_FstClusLO;
						}
						while (1)
						{
							//

							//	break;
							offsetCluster = (512 * 32) + ((newCap * 4));
							offset2 = (((newCap - 2)*capture4) + fileSec)*capture3;
							//offsetCluster = offsetCluster % capture3;
							lseek(fileNumber, offsetCluster, SEEK_SET);
							read(fileNumber, keep, 4);
							//	printf("it is =%d\n", offsetCluster);

							//	printf("it is =%d\n", capture3);
								//break;



							newCap = *(int*)keep;





							//printf("it is the count=%d\n", total);
							count3 = 0;

							count2 = 0;
							while (1)
							{
								//	printf("%s", "HELLO5");
									//printf("%d\n", offset2);
								lseek(fileNumber, offset2 + (count2 * 32), SEEK_SET);

								read(fileNumber, name, 32);
								memcpy(&direct, name, sizeof(struct DirectoryEntry));
								one = strchr(direct.DIR_Name, ' ');
								if (count2 * 32 >= 512) {
									break;
								}
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
									if (strcmp(direct.DIR_Name, ".") == 0) {
										//count3++;
									}
									if (count3 == 2) {
										//break;
									}

									printf("%s\n", direct.DIR_Name);

									//printf("%d\n", direct.DIR_FileSize);
									//printf("%d\n", offset2 + count2 * 32);
									//printf("%d\n", offsetCluster = (512 * 32) + ((direct.DIR_FstClusLO * 4)));

									count2++;
								}

							}
							if (*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8) {
								//	printf("it is4 =%d\n", offsetCluster);
								break;
							}

							//printf("%d\n", count3);
						}
						count3++;
						break;

					}
					//count++;
					else {
						count++;
					}
					//	printf("%d\n", count3);

				}
				//printf("%d\n", count3);
				if (*(int*)keep2 == 0x0FFFFFFF || *(int*)keep2 == 0x0FFFFFFE || *(int*)keep2 == 0x0FFFFFF8 || count3 != 0) {
					//printf("it is4 =%d\n", offsetCluster);
					break;
				}
			}
		}
		else if (strcmp(command, "cd") == 0) {

			int count = 0;
			int count2 = 0;
			int count3 = 0;
			int total = 0;
			int offset2 = 0;
			char keep2[4];
			struct DirectoryEntry direct;
			unsigned char *one;
			unsigned int newCap2 = capture2;
			unsigned char *one1;
			if (temp == ' ') {
				scanf("%s", cmd2);
			}
			//printf("%s\n", cmd2);
			while (1)
			{
				offsetCluster = (512 * 32) + ((newCap2 * 4));
				offset2 = (((newCap2 - 2)*capture4) + fileSec)*capture3;
				lseek(fileNumber, offsetCluster, SEEK_SET);
				read(fileNumber, keep2, 4);
				newCap2 = *(int*)keep2;
				count = 0;
				while (1)
				{

					lseek(fileNumber, offset2 + (count * 32), SEEK_SET);
					read(fileNumber, name, 32);
					memcpy(&direct, name, sizeof(struct DirectoryEntry));
					one1 = strchr(direct.DIR_Name, ' ');
					if (one1 != NULL && 11 > (one1 - direct.DIR_Name)) {
						direct.DIR_Name[(one1 - direct.DIR_Name)] = '\0';
					}
					/*	printf("%s1\n", direct.DIR_Name);
						if ((strcmp(direct.DIR_Name, cmd2) == 0)) {
							printf("%s1\n", direct.DIR_Name);
						}
						else { printf("%s2\n", direct.DIR_Name); }
						*/
					if (direct.DIR_Name[0] == 0x0) {
						//printf("%s\n", "This has a 0");
						break;
					}

					else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
						count++;
					}
					else if (direct.DIR_Attr == 0x20 && strcmp(direct.DIR_Name, cmd2) == 0) {
						//	printf("%s", "HELLO4");
						printf("%s\n", "This is a file");
						count3++;
						break;
					}
					else if ((strcmp(direct.DIR_Name, cmd2) == 0)) {
						if (strcmp("..", cmd2) == 0) {
							//	printf("%s", "HELLO3");
							rootDir--;
						}
						else {
							rootDir++;
						}

						if (((rootDir) == 0 && strcmp("..", cmd2) == 0)) {
							capture2 = *(int*)bpb6;
							//	printf("%s", "HELLO3");
						}
						else
						{
							//	printf("%s", "HELLO4");
							capture2 = direct.DIR_FstClusLO;
						}
						//capture2 = direct.DIR_FstClusLO;


						offset = (((capture2 - 2)*capture4) + fileSec) * capture3;
						break;
					}

					else {
						count++;
					}

				}
				if (*(int*)keep2 == 0x0FFFFFFF || *(int*)keep2 == 0x0FFFFFFE || *(int*)keep2 == 0x0FFFFFF8) {
					//	printf("it is4 =%d\n", offsetCluster);
					break;
				}
			}


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

			unsigned int input_mode;

			if (temp == ' ') {
				scanf("%s %u", cmd2, input_mode);
			}

			printf("This is cmd2: %s %u\n", cmd2, input_mode);

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
			int count2 = 0;
			int count3 = 0;
			int total = 0;
			int offset2 = 0;
			char keep2[4];
			struct DirectoryEntry direct;
			unsigned char *one;
			unsigned int newCap2 = capture2;
			unsigned char *one1;
			if (temp == ' ') {
				scanf("%s", cmd2);
			}
			while (1)
			{
				offsetCluster = (512 * 32) + ((newCap2 * 4));
				offset2 = (((newCap2 - 2)*capture4) + fileSec)*capture3;
				lseek(fileNumber, offsetCluster, SEEK_SET);
				read(fileNumber, keep2, 4);
				newCap2 = *(int*)keep2;
				while (1)
				{



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
					else if (strcmp(direct.DIR_Name, cmd2) == 0) {
						printf("%d\n", direct.DIR_FileSize);
						break;
					}

					else {
						count++;
					}

				}
				if (*(int*)keep2 == 0x0FFFFFFF || *(int*)keep2 == 0x0FFFFFFE || *(int*)keep2 == 0x0FFFFFF8) {
					//	printf("it is4 =%d\n", offsetCluster);
					break;
				}
			}
		}
		else if (strcmp(command, "exit") == 0)
			printf("exiting program. Good Bye\n");
		else
			printf("invalid command, try again...\n");

		memset(cmd2, 0, sizeof(cmd2));
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
