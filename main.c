#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX_OPEN_FILES 100000
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
	unsigned int DIR_Mode;
} __attribute((packed));
struct BPBEntry {
	char BPB[11];
	uint16_t BPB_BytsPerSe;
	uint8_t BPB_SecPerClus;
	uint16_t BPB_RsvdSecCnt;
	uint8_t BPB_NumFAT;
	uint16_t BPB_RootEntCnt;
	uint16_t BPB_TotSec16;
	uint8_t BPB_Media;
	uint16_t BPB_FATSz1;
	uint16_t BPB_SecPerTr;
	uint16_t BPB_NumHeads;
	uint32_t BPB_HiddSe;
	uint32_t BPB_TotSec32;
	uint32_t BPB_FATSz3;
	uint16_t BPB_ExtFla;
	uint16_t BPB_FSV;
	uint32_t  BPB_RootClus;
	char BPB1[464];
} __attribute((packed));

// Directory structure to track open/close files
typedef struct directory {
	struct DirectoryEntry files[MAX_OPEN_FILES];
	unsigned int count;
} directory;
directory openFiles;
unsigned int maxIndex;	// The maximum index of the open files

int SearchOpenFiles(char * fname);
int GetOpenFileSlot();
void PrintOpenFiles();
int clusterFromPath(char * name);
int main(int argc, char *argv[]) {
	openFiles.count = 0;
	int prevClus[100000];
	int size;
	char command[100];
	char exit[100];
	strcpy(exit, "exit");
	unsigned char bpb[3];
	unsigned char bpb1[2];
	unsigned char bpb2[3];
	unsigned char bpb3[2];
	unsigned char bpb4[5];
	unsigned char bpb5[5];
	unsigned char bpb6[5];
	unsigned char name[512];
	unsigned char read1[512];
	unsigned char bobs[1000];
	unsigned char fullname[1000];
	struct BPBEntry bob;
	//clusterFromPath("./GREEN");
	unsigned int firstSector = 0;
	//read in user input... all commands available have been implemented.
	unsigned int fileNumber = 0;
	unsigned int fileSec = 0;
	unsigned int capture = 0;
	fileNumber = open(argv[1], O_RDWR);
	read(fileNumber, bobs, 512);

	lseek(fileNumber, 44, SEEK_SET);
	read(fileNumber, bpb6, 4);
	memcpy(&bob, bobs, sizeof(struct BPBEntry));

	unsigned int capture4 = bob.BPB_SecPerClus;
	unsigned int capture3 = bob.BPB_BytsPerSe;
	unsigned int capture2 = *(int*)bpb6;
	unsigned int rsv = bob.BPB_RsvdSecCnt;
	unsigned int nFats = bob.BPB_NumFAT;
	unsigned int fSize = bob.BPB_FATSz3;

	int rootDir = 0;
	//printf("Sector per Cluser: %d\n", capture);

	fileSec = rsv + (nFats*fSize);
	firstSector = ((capture2 - 2)*capture4) + fileSec;
	unsigned  int offset = firstSector * capture3;
	unsigned  int offsetCluster;
	//lseek(fileNumber, 0, SEEK_SET);


	while (strcmp(command, "exit") != 0) {
		char cmd2[100];
		printf("$ ");
		char temp = '\0';
		scanf("%s", command);
		temp = getchar();

		if (strcmp(command, "info") == 0) {


			printf("Bytes per sec: %d\n", bob.BPB_BytsPerSe);

			printf("Sector per Cluser: %d\n", bob.BPB_SecPerClus);


			printf("reseverd sector count: %d\n", bob.BPB_RsvdSecCnt);



			printf("number of FATs: %d\n", bob.BPB_NumFAT);


			printf("total sectors %d\n", bob.BPB_TotSec32);


			printf("FATsize: %d\n", bob.BPB_FATSz3);


			capture = *(int*)bpb6;
			printf("FAT Root Cluster: %d\n", capture);
			offset = (((587 - 2)*capture4) + fileSec) * capture3;
			//printf("%d", offset);

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
			//printf("%d\n",capture2);
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
				offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap2 * 4));
				offset = (((newCap2 - 2)*capture4) + fileSec)*capture3;
				lseek(fileNumber, offsetCluster, SEEK_SET);
				read(fileNumber, keep2, 4);
				newCap = *(int*)keep2;
				count = 0;
				//	printf("stuck");
				while (1)
				{
					//	printf("%s", "HELLO2");
					//	printf("stuck2");

					//	offsetCluster = (bob.BPB_BytsPerSe * 32) + (capture2 * 4);
					lseek(fileNumber, offset + (count * 32), SEEK_SET);
					read(fileNumber, name, 32);
					memcpy(&direct, name, sizeof(struct DirectoryEntry));
					//	printf("%s\n", direct.DIR_Name);
					one = strchr(direct.DIR_Name, ' ');
					if (one != NULL && 11 > (one - direct.DIR_Name)) {
						direct.DIR_Name[(one - direct.DIR_Name)] = '\0';

					}

					if ((direct.DIR_Name[0] == 0x0) || (rootDir == 0 && strcmp("..", cmd2) == 0)) {
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
						//printf("%s", "HELLO5");
						count++;
					}
					else if (strcmp(direct.DIR_Name, cmd2) == 0 || strcmp("", cmd2) == 0 || ((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
						//printf("%s\n",cmd2);

						//		printf("%d\n", direct.DIR_FstClusHI);
						//		printf("%d\n", direct.DIR_FstClusLO);

					//	printf("%s", "HELLO1");
								//break;
					//	printf("%s", "HELLO5");
						if (strcmp("", cmd2) == 0) {
							//	printf("%s", "HELLO4");
							newCap = capture2;
						}
						else if (((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
							newCap = *(int*)bpb6;
							//printf("%s", "HELLO3");
						}
						else
						{
							//printf("%s", "HELLO4");
							newCap = direct.DIR_FstClusLO + (direct.DIR_FstClusHI << 16);
						}
						while (1)
						{
							//
						//	printf("%s", "HELLO5");
							//	break;
							offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
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
								if (count2 * 32 >= bob.BPB_BytsPerSe) {
									break;
								}
								if (one != NULL && 11 > (one - direct.DIR_Name)) {
									direct.DIR_Name[(one - direct.DIR_Name)] = '\0';
								}

								if (direct.DIR_Name[0] == 0x0) {
									//	printf("O\n");
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
									//printf("%d\n", offsetCluster = (bob.BPB_BytsPerSe * 32) + ((direct.DIR_FstClusLO * 4)));

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
			//printf("%d\n", count3);
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
				offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap2 * 4));
				offset2 = (((newCap2 - 2)*capture4) + fileSec)*capture3;
				lseek(fileNumber, offsetCluster, SEEK_SET);
				read(fileNumber, keep2, 4);
				newCap2 = *(int*)keep2;
				count = 0;
				count3 = 0;
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
					if (count * 32 >= bob.BPB_BytsPerSe) {
						break;
					}
					if (direct.DIR_Name[0] == 0x0) {
						printf("%s\n", "This Ditrectory doesnt exist");
						count3++;
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
						//printf("%s %d\n", "This is a file", rootDir);
						if (((rootDir) == 0 && strcmp("..", cmd2) == 0)) {
							capture2 = *(int*)bpb6;
							count3++;
							//break;
							//printf("%s", "HELLO3");
						}
						else if (strcmp("..", cmd2) == 0) {
							capture2 = prevClus[size];
							prevClus[size] = 0;
							size--;
							count3++;
							//break;
						}
						else
						{
							size++;
							//	printf("%s", "HELLO4");
							prevClus[size] = capture2;

							capture2 = direct.DIR_FstClusLO + (direct.DIR_FstClusHI << 16);
							count3++;
							break;
						}
						//capture2 = direct.DIR_FstClusLO;


						offset = (((capture2 - 2)*capture4) + fileSec) * capture3;
						break;
					}

					else {
						count++;
					}

				}
				if (*(int*)keep2 == 0x0FFFFFFF || *(int*)keep2 == 0x0FFFFFFE || *(int*)keep2 == 0x0FFFFFF8 || count3 != 0) {
					//	printf("it is4 =%d\n", offsetCluster);
					break;
				}
			}



		}
		else if (strcmp(command, "creat") == 0) {
			int count = 0;
			int count2 = 0;
			int count3 = 0;
			int count4 = 0;
			int count5 = 0;
			int k = 0;
			int total = 0;
			int offset2 = 0;
			int offset3 = 0;
			int offsetCluster2 = 0;
			int dont = 0;
			int wannabe = 0x0FFFFFF8;
			char keep[4];
			char keep2[4];
			char keep5[4];
			char keep6[32];
			unsigned char stop[10000];
			char picks[1000];
			int free = 0;
			//	printf("%s\n", "HELLO1");
			memset(stop, '\0', sizeof(stop));
			//memset(free, '\0', sizeof(free));
			char * name2;
			struct DirectoryEntry direct;
			struct DirectoryEntry empty;
			struct DirectoryEntry create;
			struct DirectoryEntry holding;
			unsigned char *one;
			unsigned int newCap = capture2;
			unsigned int newCap2 = capture2;
			unsigned int newCap3 = 0;
			int directoryOffset = 0;
			unsigned char returning[32];
			unsigned int firstCLUSter = 0;
			unsigned int fileSize = 0;
			int safe = 0;





			if (temp != ' ') {

			}
			else {
				scanf("%s", cmd2);
				while (1)
				{
					//
					//printf("%s\n", "HELLO6	23123");
					//	break;
					offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
					offset2 = (((newCap - 2)*capture4) + fileSec)*capture3;
					//offsetCluster = offsetCluster % capture3;
					lseek(fileNumber, offsetCluster, SEEK_SET);
					read(fileNumber, keep, 4);
					//	printf("it is =%d\n", offsetCluster);

						//printf("it is =%d\n", capture3);
						//break;



					newCap = *(int*)keep;





					//printf("it is the count=%d\n", total);
					count3 = 0;

					count2 = 0;
					while (1)
					{
						//printf("%s\n", "HELLO7");
						//	printf("%s", "HELLO5");
							//printf("%d\n", offset2);
						lseek(fileNumber, offset2 + (count2 * 32), SEEK_SET);

						read(fileNumber, name, 32);
						memcpy(&direct, name, sizeof(struct DirectoryEntry));
						one = strchr(direct.DIR_Name, ' ');
						if (count2 * 32 >= bob.BPB_BytsPerSe) {
							break;
						}
						if (one != NULL && 11 > (one - direct.DIR_Name)) {
							direct.DIR_Name[(one - direct.DIR_Name)] = '\0';
						}

						if (direct.DIR_Name[0] == 0x0 && count2 * 32 < bob.BPB_BytsPerSe) {
							directoryOffset = offset2 + (count2 * 32);
							count4++;
							break;
						}
						else if (direct.DIR_Name[0] == 0xE5) {
							directoryOffset = offset2 + (count2 * 32);
							count4++;
							break;
							//count2++;
						}
						else {
							if (strcmp(direct.DIR_Name, ".") == 0) {
								//count3++;
							}
							if (count3 == 2) {
								//break;
							}

							count2++;
						}

					}
					if (count4 != 0) {

						count4 = 0; break;
					}
					else if (*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8) {
						//	printf("it is4 =%d\n", offsetCluster);
						for (k = 0; 1; k++) {
							offsetCluster2 = (bob.BPB_BytsPerSe * 32) + (((*(int*)bpb6 + k) * 4));
							//printf("%s %d\n", "HELLO1", k);
							lseek(fileNumber, offsetCluster2, SEEK_SET);
							read(fileNumber, keep5, 4);
							if (*(int*)keep5 == 0) {
								//	printf("%s %d\n", "HELLO2bb", k);
								lseek(fileNumber, offsetCluster2, SEEK_SET);
								write(fileNumber, &wannabe, 4);
								lseek(fileNumber, offsetCluster, SEEK_SET);
								directoryOffset = (((k - 2)*capture4) + fileSec)*capture3;
								write(fileNumber, &k, 4);
								newCap = k;
								count3++;
								break;

							}
						}
						break;
					}


					//printf("%d\n", count3);
				}
				//printf("cp entered\n");
				strncpy(create.DIR_Name, cmd2, 8);
				unsigned short high = firstCLUSter >> 16;
				unsigned short low = ((firstCLUSter << 16) >> 16);
				create.DIR_Attr = 0X20;
				create.DIR_FileSize = 0;
				offset = (((low - 2)*capture4) + fileSec)*capture3;
				//printf("%d\n", offset);
				//printf("%d\n", low);
				create.DIR_FstClusHI = high;
				create.DIR_FstClusLO = low;
				lseek(fileNumber, directoryOffset, SEEK_SET);

				memcpy(keep6, &create, sizeof(struct DirectoryEntry));
				//memset(keep6);
				write(fileNumber, keep6, 32);











			}



			printf("creat entered\n");
		}
		else if (strcmp(command, "mkdir") == 0) {
			int count = 0;
			int count2 = 0;
			int count3 = 0;
			int count4 = 0;
			int count5 = 0;
			int k = 0;
			int total = 0;
			int offset2 = 0;
			int offset3 = 0;
			int offsetCluster2 = 0;
			int dont = 0;
			int wannabe = 0x0FFFFFF8;
			char keep[4];
			char keep2[4];
			char keep5[4];
			char keep6[32];
			unsigned char stop[10000];
			char picks[1000];
			int free = 0;
			//	printf("%s\n", "HELLO1");
			memset(stop, '\0', sizeof(stop));
			//memset(free, '\0', sizeof(free));
			char * name2;
			struct DirectoryEntry direct;
			struct DirectoryEntry empty;
			struct DirectoryEntry create;
			struct DirectoryEntry holding;
			unsigned char *one;
			unsigned int newCap = capture2;
			unsigned int newCap2 = capture2;
			unsigned int newCap3 = 0;
			int directoryOffset = 0;
			unsigned char returning[32];
			unsigned int firstCLUSter = 0;
			unsigned int fileSize = 0;
			int safe = 0;





			if (temp != ' ') {

			}
			else {
				//printf("%s\n", "HELLO6	23123");
				scanf("%s", cmd2);
				while (1)
				{
					//
					//printf("%s\n", "HELLO6	23123");
					//	break;
					offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
					offset2 = (((newCap - 2)*capture4) + fileSec)*capture3;
					//offsetCluster = offsetCluster % capture3;
					lseek(fileNumber, offsetCluster, SEEK_SET);
					read(fileNumber, keep, 4);
					//	printf("it is =%d\n", offsetCluster);

						//printf("it is =%d\n", capture3);
						//break;



					newCap = *(int*)keep;





					//printf("it is the count=%d\n", total);
					count3 = 0;

					count2 = 0;
					while (1)
					{
						//printf("%s\n", "HELLO7");
						//	printf("%s", "HELLO5");
							//printf("%d\n", offset2);
						lseek(fileNumber, offset2 + (count2 * 32), SEEK_SET);

						read(fileNumber, name, 32);
						memcpy(&direct, name, sizeof(struct DirectoryEntry));
						one = strchr(direct.DIR_Name, ' ');
						if (count2 * 32 >= bob.BPB_BytsPerSe) {
							break;
						}
						if (one != NULL && 11 > (one - direct.DIR_Name)) {
							direct.DIR_Name[(one - direct.DIR_Name)] = '\0';
						}

						if (direct.DIR_Name[0] == 0x0 && count2 * 32 < bob.BPB_BytsPerSe) {
							directoryOffset = offset2 + (count2 * 32);
							count4++;
							break;
						}
						else if (direct.DIR_Name[0] == 0xE5) {
							directoryOffset = offset2 + (count2 * 32);
							count4++;
							break;
							//count2++;
						}
						else {
							if (strcmp(direct.DIR_Name, ".") == 0) {
								//count3++;
							}
							if (count3 == 2) {
								//break;
							}

							count2++;
						}

					}
					if (count4 != 0) {

						count4 = 0; break;
					}
					else if (*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8) {
						//	printf("it is4 =%d\n", offsetCluster);
						for (k = 0; 1; k++) {
							offsetCluster2 = (bob.BPB_BytsPerSe * 32) + (((*(int*)bpb6 + k) * 4));
							//printf("%s %d\n", "HELLO1", k);
							lseek(fileNumber, offsetCluster2, SEEK_SET);
							read(fileNumber, keep5, 4);
							if (*(int*)keep5 == 0) {
								//	printf("%s %d\n", "HELLO2bb", k);
								lseek(fileNumber, offsetCluster2, SEEK_SET);
								write(fileNumber, &wannabe, 4);
								lseek(fileNumber, offsetCluster, SEEK_SET);
								directoryOffset = (((k - 2)*capture4) + fileSec)*capture3;
								write(fileNumber, &k, 4);
								newCap = k;
								count3++;
								break;

							}
						}
						break;
					}


					//printf("%d\n", count3);
				}
				//printf("cp entered\n");
				strncpy(create.DIR_Name, cmd2, 8);
				unsigned short high = firstCLUSter >> 16;
				unsigned short low = ((firstCLUSter << 16) >> 16);
				create.DIR_Attr = 0X10;
				create.DIR_FileSize = 0;
				offset = (((low - 2)*capture4) + fileSec)*capture3;
				//printf("%d\n", offset);
				//printf("%d\n", low);
				create.DIR_FstClusHI = high;
				create.DIR_FstClusLO = low;
				lseek(fileNumber, directoryOffset, SEEK_SET);

				memcpy(keep6, &create, sizeof(struct DirectoryEntry));
				//memset(keep6);
				write(fileNumber, keep6, 32);


				strncpy(create.DIR_Name, ".", 8);
				offset2 = (((firstCLUSter - 2)*capture4) + fileSec)*capture3;
				lseek(fileNumber, offset2, SEEK_SET);
				memcpy(keep6, &create, sizeof(struct DirectoryEntry));
				write(fileNumber, keep6, 32);
				strncpy(create.DIR_Name, "..", 8);
				create.DIR_FstClusHI = capture2 >> 16;
				create.DIR_FstClusLO = ((capture2 << 16) >> 16);
				memcpy(keep6, &create, sizeof(struct DirectoryEntry));
				write(fileNumber, keep6, 32);


			}
			printf("mkdir entered\n");
		}
		else if (strcmp(command, "mv") == 0) {
			int count = 0;
			int count2 = 0;
			int count3 = 0;
			int count4 = 0;
			int count5 = 0;
			int k = 0;
			int finnish = 0;
			int total = 0;
			int offset2 = 0;
			int offset3 = 0;
			int directory = 0;
			int directory2 = 0;
			int offsetCluster2 = 0;
			int wannabe = 0x0FFFFFF8;
			char keep[4];
			char keep2[4];
			char keep5[4];
			char keep6[32];
			unsigned char stop[10000];
			char picks[1000];
			int free = 0;
			//	printf("%s\n", "HELLO1");
			memset(stop, '\0', sizeof(stop));
			//memset(free, '\0', sizeof(free));
			char * name2;
			struct DirectoryEntry direct;
			struct DirectoryEntry empty;
			struct DirectoryEntry create;
			struct DirectoryEntry holding;
			unsigned char *one;
			unsigned int newCap = capture2;
			unsigned int newCap2 = capture2;
			unsigned int newCap3 = 0;
			int directoryOffset = 0;
			int directoryOffset2 = 0;
			unsigned char returning[32];
			unsigned int firstCLUSter;
			unsigned int fileSize;
			int file = 0;
			int safe = 0;
			//printf("%s\n", "HELLO1");
			if (temp != ' ') {

			}
			else {
				scanf("%s %s", cmd2, picks);
				//printf("%s\n", "HELLO2");
				while (1)
				{


					offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap2 * 4));
					offset2 = (((newCap2 - 2)*capture4) + fileSec)*capture3;
					lseek(fileNumber, offsetCluster, SEEK_SET);
					read(fileNumber, keep2, 4);
					newCap2 = *(int*)keep2;
					count = 0;
					count3 = 0;
					while (1)
					{
						//printf("%s\n", "HELLO4");
							//	printf("%s", "HELLO2");
							//	printf("stuck2");
						lseek(fileNumber, offset2 + (count * 32), SEEK_SET);

						read(fileNumber, name, 32);
						memcpy(&direct, name, sizeof(struct DirectoryEntry));

						//printf("%s %s\n", direct.DIR_Name, "dklsdk");
						one = strchr(direct.DIR_Name, ' ');
						if (one != NULL && 11 > (one - direct.DIR_Name)) {
							direct.DIR_Name[(one - direct.DIR_Name)] = '\0';

						}
						//printf("%s\n", direct.DIR_Name);
						if (count * 32 >= bob.BPB_BytsPerSe) {
							break;
						}
						if ((direct.DIR_Name[0] == 0x0) || (rootDir == 0 && strcmp("..", picks) == 0)) {
							//	printf("%s", "HELLO4");
							file = 2;

							//printf("%s\n", "Directory doesnt exist");
							count3++;
							break;
						}


						else if (direct.DIR_Attr == 0x20 && strcmp(direct.DIR_Name, picks) == 0) {

							directoryOffset2 = offset2 + (count * 32);
							file = 1;
							count3++;
							break;
						}
						else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {

							count++;
						}
						else if (strcmp(direct.DIR_Name, picks) == 0 || strcmp("", picks) == 0 || ((rootDir - 1) == 0 && strcmp("..", picks) == 0) || ((rootDir) == 0 && strcmp(".", picks) == 0)) {
							//printf("%s\n",cmd2);
							finnish = 1;

							safe = 1;
							//		printf("%d\n", direct.DIR_FstClusHI);
							//		printf("%d\n", direct.DIR_FstClusLO);

						//	printf("%s", "HELLO1");
									//break;
						//	printf("%s", "HELLO5");
							if (strcmp("", cmd2) == 0) {
								//	printf("%s", "HELLO4");
								newCap = capture2;
							}
							else if (((rootDir - 1) == 0 && strcmp("..", picks) == 0) || ((rootDir) == 0 && strcmp(".", picks) == 0)) {
								newCap = *(int*)bpb6;
								//printf("%s", "HELLO3");
							}
							else
							{
								//printf("%s", "HELLO4");
								newCap = direct.DIR_FstClusLO + (direct.DIR_FstClusHI << 16);
							}
							while (1)
							{
								//
								//printf("%s\n", "HELLO6");
								//	break;
								offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
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
									//printf("%s\n", "HELLO7");
									//	printf("%s", "HELLO5");
										//printf("%d\n", offset2);
									lseek(fileNumber, offset2 + (count2 * 32), SEEK_SET);

									read(fileNumber, name, 32);
									memcpy(&direct, name, sizeof(struct DirectoryEntry));
									one = strchr(direct.DIR_Name, ' ');
									if (count2 * 32 >= bob.BPB_BytsPerSe) {
										break;
									}
									if (one != NULL && 11 > (one - direct.DIR_Name)) {
										direct.DIR_Name[(one - direct.DIR_Name)] = '\0';
									}

									if (direct.DIR_Name[0] == 0x0 && count2 * 32 < bob.BPB_BytsPerSe) {
										directoryOffset = offset2 + (count2 * 32);
										count4++;
										break;
									}
									else if (direct.DIR_Name[0] == 0xE5) {
										directoryOffset = offset2 + (count2 * 32);
										count4++;
										break;
										count2++;
									}
									else {
										if (strcmp(direct.DIR_Name, ".") == 0) {
											//count3++;
										}
										if (count3 == 2) {
											//break;
										}

										count2++;
									}

								}
								if (count4 != 0) {

									count4 = 0; break;
								}
								else if (*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8) {
									//	printf("it is4 =%d\n", offsetCluster);
									for (k = 0; 1; k++) {
										offsetCluster2 = (bob.BPB_BytsPerSe * 32) + (((*(int*)bpb6 + k) * 4));
										//printf("%s %d\n", "HELLO1", k);
										lseek(fileNumber, offsetCluster2, SEEK_SET);
										read(fileNumber, keep5, 4);
										if (*(int*)keep5 == 0) {
											//	printf("%s %d\n", "HELLO2", k);
											lseek(fileNumber, offsetCluster2, SEEK_SET);
											write(fileNumber, &wannabe, 4);
											lseek(fileNumber, offsetCluster, SEEK_SET);
											write(fileNumber, &k, 4);
											newCap = k;
											break;
											//cout3++;
										}
									}
									break;
								}

								//printf("%d\n", count3);
							}
							count3++;
							break;

						}
						//count++;
						else {
							//printf("hekki");
							count++;
						}
					}
					//	printf("%d\n", count3);
					if (*(int*)keep2 == 0x0FFFFFFF || *(int*)keep2 == 0x0FFFFFFE || *(int*)keep2 == 0x0FFFFFF8 || count3 != 0) {
						//printf("it is4 =%d\n", offsetCluster);
						break;
					}









				}















				unsigned int newCap = capture2;
				unsigned int newCap2 = capture2;

				count3 = 0;

				while (1)
				{

					//	printf("%s", "HELLO1");
					offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
					offset = (((newCap - 2)*capture4) + fileSec)*capture3;
					lseek(fileNumber, offsetCluster, SEEK_SET);
					read(fileNumber, keep2, 4);
					newCap = *(int*)keep2;
					count = 0;
					//	printf("stuck");
					while (1)
					{
						//printf("%s\n", "HELLO4");
						//	printf("%s", "HELLO2");
						//	printf("stuck2");

						//	offsetCluster = (bob.BPB_BytsPerSe * 32) + (capture2 * 4);
						lseek(fileNumber, offset + (count * 32), SEEK_SET);
						read(fileNumber, name, 32);
						memcpy(&direct, name, sizeof(struct DirectoryEntry));

						one = strchr(direct.DIR_Name, ' ');
						if (one != NULL && 11 > (one - direct.DIR_Name)) {
							direct.DIR_Name[(one - direct.DIR_Name)] = '\0';

						}
						//printf("%s1\n", direct.DIR_Name);
						if ((direct.DIR_Name[0] == 0x0) || (rootDir == 0 && strcmp("..", cmd2) == 0)) {
							if (finnish <= 1) {
								directory2 = 1;
							}

							count3++;
							break;
						}

						if (count * 32 >= bob.BPB_BytsPerSe) {
							//printf("%s\n", "Directory doesnt exist");
							break;
						}

						else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
							//printf("%s", "HELLO5");
							count++;
						}
						else if (strcmp(direct.DIR_Name, cmd2) == 0 || strcmp("", cmd2) == 0 || ((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
							//printf("%s\n",cmd2);
							if (direct.DIR_Attr == 0x10 && strcmp(direct.DIR_Name, cmd2) == 0 && file == 1) {
								directory = 1;
								count3++;
								break;
							}
							if (direct.DIR_Attr == 0x10) {
								directory2 = 1;
								//printf("%s", "HELLO1");
							}

							safe = 1;
							//		printf("%d\n", direct.DIR_FstClusHI);
							//		printf("%d\n", direct.DIR_FstClusLO);

						//	printf("%s", "HELLO1");
									//break;
						//	printf("%s", "HELLO5");
							/*if (strcmp("", cmd2) == 0) {
								//	printf("%s", "HELLO4");
								newCap = capture2;
							}
							else if (((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
								newCap = *(int*)bpb6;
								printf("%s", "HELLO3");
							}
							else
							{
								printf("%s", "HELLO4");
								//newCap = direct.DIR_FstClusLO + (direct.DIR_FstClusHI << 16);
							}*/

							//
							//printf("%s\n", "HELLO6");
							//	break;
							//offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
							//offset2 = (((newCap - 2)*capture4) + fileSec)*capture3;
							//offsetCluster = offsetCluster % capture3;
							//lseek(fileNumber, offsetCluster, SEEK_SET);
							//read(fileNumber, keep, 4);
							//	printf("it is =%d\n", offsetCluster);

							//	printf("it is =%d\n", capture3);
								//break;



							//newCap = *(int*)keep;

							if (finnish == 1) {
								finnish = 2;
							}


							directoryOffset2 = offset + (count * 32);

							//printf("it is the count=%d\n", total);
							count3 = 0;

							count2 = 0;
							lseek(fileNumber, offset + ((count + 1) * 32), SEEK_SET);
							read(fileNumber, name, 32);
							memcpy(&holding, name, sizeof(struct DirectoryEntry));
							//printf("%s\n", "HELLO7");
							//	printf("%s", "HELLO5");
								//printf("%d\n", offset2);
						//	lseek(fileNumber, offset2 + ((count2 +1)* 32), SEEK_SET);

							//read(fileNumber, name, 32);
						//	memcpy(&direct, name, sizeof(struct DirectoryEntry));
							one = strchr(direct.DIR_Name, ' ');
							if (count * 32 >= bob.BPB_BytsPerSe) {
								//printf("%s\n", "Directory doesnt exist");
								count4++;
								break;
							}
							if (one != NULL && 11 > (one - holding.DIR_Name)) {


								direct.DIR_Name[(one - direct.DIR_Name)] = '\0';
							}

							if (holding.DIR_Name[0] == 0x0 || count * 32 > bob.BPB_BytsPerSe) {
								empty.DIR_Name[0] = 0x0;

								directoryOffset2 = offset + (count * 32);
								count4++;
								break;
							}
							else {
								empty.DIR_Name[0] = 0xE5;

								directoryOffset2 = offset + (count * 32);
								count4++;
								break;
								//count2++;
							}




							//printf("%d\n", count3);

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
				//printf("%d\n", count3);



				newCap = capture2;
				newCap2 = capture2;
				count3 = 0;
				count4 = 0;















				if (file == 1 && directory == 1) {

					printf("%s\n", "You can't put a directory into a file");
				}
				else if (file == 1) {

					printf("%s\n", "This is another files name");
				}
				else if (file == 2 && directory2 != 1) {
					//printf("this is %d\n", finnish);
					strncpy(direct.DIR_Name, picks, 8);
					lseek(fileNumber, directoryOffset2, SEEK_SET);
					memcpy(returning, &direct.DIR_Name, sizeof(struct DirectoryEntry));
					write(fileNumber, returning, 32);
				}
				else if (finnish >= 2) {
					//printf("thissds is %d\n", finnish);
					lseek(fileNumber, directoryOffset2, SEEK_SET);
					read(fileNumber, name, 32);

					lseek(fileNumber, directoryOffset2, SEEK_SET);
					memcpy(returning, &empty, sizeof(struct DirectoryEntry));
					if (empty.DIR_Name[0] == 0xE5) {

					}
					write(fileNumber, returning, 32);
					lseek(fileNumber, directoryOffset, SEEK_SET);
					write(fileNumber, name, 32);
				}
				else {
					printf("%s\n", "Directory/File doesnt exist");
				}

				//	printf("mv entered %d\n", directoryOffset);
				//	printf("mv entered %d\n", directoryOffset2);

			}
		}
		else if (strcmp(command, "open") == 0) {


			char  file_name[20];
			char  input_mode[3];

			if (temp == ' ') {
				scanf("%s %s", file_name, input_mode);
			}

			if (strcmp(input_mode, "r") == 0 || strcmp(input_mode, "w") == 0 ||
				strcmp(input_mode, "rw") == 0 || strcmp(input_mode, "wr") == 0) {

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

					strcpy(cmd2, "");

					while (1)
					{
						offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap2 * 4));
						offset2 = (((newCap2 - 2)*capture4) + fileSec)*capture3;
						lseek(fileNumber, offsetCluster, SEEK_SET);
						read(fileNumber, keep2, 4);
						newCap = *(int*)keep2;
						count = 0;
						while (1)
						{
							lseek(fileNumber, offset + (count * 32), SEEK_SET);
							read(fileNumber, name, 32);
							memcpy(&direct, name, sizeof(struct DirectoryEntry));
							one = strchr(direct.DIR_Name, ' ');
							if (one != NULL && 11 > (one - direct.DIR_Name)) {
								direct.DIR_Name[(one - direct.DIR_Name)] = '\0';

							}
							if (strcmp(direct.DIR_Name, cmd2) == 0 || strcmp("", cmd2) == 0 || ((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
								if (strcmp("", cmd2) == 0) {
									newCap = capture2;
								}
								while (1)
								{
									offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
									offset2 = (((newCap - 2)*capture4) + fileSec)*capture3;
									lseek(fileNumber, offsetCluster, SEEK_SET);
									read(fileNumber, keep, 4);
									newCap = *(int*)keep;
									count3 = 0;
									count2 = 0;
									while (1)
									{
										lseek(fileNumber, offset2 + (count2 * 32), SEEK_SET);
										read(fileNumber, name, 32);
										memcpy(&direct, name, sizeof(struct DirectoryEntry));
										one = strchr(direct.DIR_Name, ' ');
										if (count2 * 32 >= bob.BPB_BytsPerSe) {
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
											if (direct.DIR_Attr == 0x20 && (strcmp(direct.DIR_Name, file_name) == 0)) {
												if (SearchOpenFiles(file_name) == MAX_OPEN_FILES) {
													// File not opened

													struct DirectoryEntry * newEntry;

													newEntry = &openFiles.files[GetOpenFileSlot()];
													//printf("Made it to here\n");
													strcpy(newEntry->DIR_Name, direct.DIR_Name);
													newEntry->DIR_Attr = direct.DIR_Attr;
													newEntry->DIR_NTRes = direct.DIR_NTRes;
													newEntry->DIR_CrtTimeTenth = direct.DIR_CrtTimeTenth;
													newEntry->DIR_CrtTime = direct.DIR_CrtTime;
													newEntry->DIR_CrtDate = direct.DIR_CrtDate;
													newEntry->DIR_LstAccDate = direct.DIR_LstAccDate;
													newEntry->DIR_FstClusHI = direct.DIR_FstClusHI;
													newEntry->DIR_WrtTime = direct.DIR_WrtTime;
													newEntry->DIR_WrtDate = direct.DIR_WrtDate;
													newEntry->DIR_FstClusLO = direct.DIR_FstClusLO;
													newEntry->DIR_FileSize = direct.DIR_FileSize;
													if (strcmp(input_mode, "r") == 0) {
														direct.DIR_Mode = 1;
													}
													else if (strcmp(input_mode, "w") == 0) {
														direct.DIR_Mode = 2;
													}
													else if (strcmp(input_mode, "rw") == 0){
														direct.DIR_Mode = 3;
													}
													else if (strcmp(input_mode, "wr") == 0){
														direct.DIR_Mode = 4;
													}
													else {
														printf("MODE INPUT ERROR\n");
													}
													newEntry->DIR_Mode = direct.DIR_Mode;
													openFiles.count++;

													printf("%s successfully opened\n", newEntry->DIR_Name);
												}
												else {
													// File already exists
													fprintf(stderr, "%s already has been opened\n", file_name);
													continue;
												}
												count2++;
											}
											count2++;
										}
									}
									if (*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8) {
										break;
									}
								}
								count3++;
								break;
							}
							else {
								count++;
							}
						}
						if (*(int*)keep2 == 0x0FFFFFFF || *(int*)keep2 == 0x0FFFFFFE || *(int*)keep2 == 0x0FFFFFF8 || count3 != 0) {
							break;
						}
					}
			}
			else {
				fprintf(stderr, "%s is not a valid mode input\n", input_mode);
				continue;
			}

			PrintOpenFiles();
		}
		else if (strcmp(command, "close") == 0) {

			char  file_name[20];

			if (temp == ' ') {
				scanf("%s", file_name);
			}

				int lookupIndex;
				lookupIndex = SearchOpenFiles(file_name);
				if (lookupIndex < MAX_OPEN_FILES) {
					struct DirectoryEntry * newEntry;

					newEntry = &openFiles.files[lookupIndex];
					newEntry->DIR_Name[0] = '\0';
					newEntry->DIR_Attr = '\0';
					newEntry->DIR_NTRes = '\0';
					newEntry->DIR_CrtTimeTenth = '\0';
					newEntry->DIR_CrtTime = 0;
					newEntry->DIR_CrtDate = 0;
					newEntry->DIR_LstAccDate = 0;
					newEntry->DIR_FstClusHI = 0;
					newEntry->DIR_WrtTime = 0;
					newEntry->DIR_WrtDate = 0;
					newEntry->DIR_FstClusLO = 0;
					newEntry->DIR_FileSize = 0;
					newEntry->DIR_Mode = 0;
					openFiles.count--;

					printf("%s successfully closed\n", file_name);
				}
				else {
					fprintf(stderr, "%s is not opened\n", file_name);
				}

			PrintOpenFiles();

		}
		else if (strcmp(command, "read") == 0) {
			int srlCheck = 0;
			int count = 0;
			int count2 = 0;
			int count3 = 0;
			int total = 0;
			int offset2 = 0;
			int fileSize = 0;
			int offsetSize = 513;
			int sizeRead = 20;
			char keep[4];
			char keep3[4];
			char keep4[4];
			char keep2[4];
			char * name2;
			struct DirectoryEntry direct;
			unsigned char *one;
			unsigned int newCap = capture2;
			unsigned int newCap2 = capture2;

			if (temp != ' ') {

			}
			else {
				scanf("%s %s %s", cmd2, keep3, keep4);

				offsetSize = atoi(keep3);
				sizeRead = atoi(keep4);

				//printf("%d\n", sizeRead);
				//break
					/*	if (strcmp("", cmd2) == 0) {
							printf("%s\n", "File Does Not exist");
							memset(cmd2, 0, sizeof(cmd2));
						}*/
						//printf("%s\n", cmd2);

					//	break;
				while (1)
				{
					//	printf("%d\n", offsetSize);
					//	printf("%d\n", sizeRead);
						//	printf("%s", "HELLO1");
					offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap2 * 4));
					offset = (((newCap2 - 2)*capture4) + fileSec)*capture3;
					lseek(fileNumber, offsetCluster, SEEK_SET);
					read(fileNumber, keep2, 4);
					newCap2 = *(int*)keep2;
					count = 0;
					//printf("stuck");
					while (1)
					{
						//	printf("%d\n", offsetSize);
						//	printf("%d\n", sizeRead);
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
						if (count * 32 >= bob.BPB_BytsPerSe) {
							break;
						}
						if (direct.DIR_Name[0] == 0x0 || (rootDir == 0 && strcmp("..", cmd2) == 0)) {
							//	printf("%s", "HELLO4");
							printf("%s \n", "FILE doesnt exist");
							count3++;
							break;
						}


						else if (direct.DIR_Attr == 0x10 && strcmp(direct.DIR_Name, cmd2) == 0) {
							//	printf("%s", "HELLO4");
							printf("%s\n", "This is a Directory");
							count3++;
							break;
						}
						else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
							//	printf("%s", "HELLO5");
							count++;
						}
						else if (strcmp(direct.DIR_Name, cmd2) == 0 || strcmp("", cmd2) == 0 || ((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {

							int indexFinder = SearchOpenFiles(direct.DIR_Name);
							if (indexFinder < MAX_OPEN_FILES && (openFiles.files[indexFinder].DIR_Mode == 1 || openFiles.files[indexFinder].DIR_Mode == 3 || openFiles.files[indexFinder].DIR_Mode == 4)) {
								srlCheck = 1;
								//printf("%s\n",cmd2);

								//		printf("%d\n", direct.DIR_FstClusHI);
								//		printf("%d\n", direct.DIR_FstClusLO);

								fileSize = direct.DIR_FileSize;
								//	printf("%d\n", fileSize);
									//break;
						//printf("%s", "HELLO5");

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
									newCap = direct.DIR_FstClusLO + (direct.DIR_FstClusHI << 16);
								}
								while (1)
								{
									//
									//printf("%d\n", offsetSize);
								//	printf("%d\n", sizeRead);
									//printf("%d\n", fileSize);
									//break;
									//	break;
									offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
									offset2 = (((newCap - 2)*capture4) + fileSec)*capture3;
									//offsetCluster = offsetCluster % capture3;
									lseek(fileNumber, offsetCluster, SEEK_SET);
									read(fileNumber, keep, 4);
									//	printf(" =%d\n", offsetCluster);

								//	printf("it is =%d\n", offset2);
									//break;



									newCap = *(int*)keep;



									count3 = 0;
									if (fileSize < offsetSize) {
										count3++;
										break;

									}
									else if (fileSize < offsetSize + sizeRead) {
										sizeRead = fileSize - offsetSize;
										//printf("%s\n", "lllllllllllllllllllllllllllllllllllllllllll");
									}
									//   printf("it is the count=%d\n", total);
										//printf("%d\n", offsetSize);
									//	printf("%d\n", sizeRead);

									count2 = 0;
									if (bob.BPB_BytsPerSe < offsetSize) {
										offsetSize = offsetSize - bob.BPB_BytsPerSe;
										//	printf("%s\n", "lllllllllllllllllllllllllllllllllllllllllll");
									}
									else {
										//printf("%s", "HELLO5");
											//printf("%d\n", offset2);
										lseek(fileNumber, offset2 + offsetSize, SEEK_SET);

										//	printf("%d", sizeRead);
										if (512 < sizeRead + offsetSize) {
											name2 = (char*)malloc(((bob.BPB_BytsPerSe + 1) - offsetSize) * sizeof(char));
											read(fileNumber, name2, (bob.BPB_BytsPerSe - offsetSize));

											sizeRead = sizeRead - (bob.BPB_BytsPerSe - offsetSize);
											//printf("%d\n\n\n\n\n\n\n", sizeRead);
											fwrite(name2, sizeof(char), 512 - offsetSize, stdout);
											name2[512 - offsetSize] = '\0';
											//	memset(bpb4, 0, sizeof(bpb4));
											offsetSize = 0;
											//	printf("%s\n", "lllllllllllllllllllllllllllllllllllllllllll");
											//		printf("%s", "HELLO5");

										}
										else
										{
											//	printf("%s", "HELLO6");
											//	printf("%d", sizeRead);
												//printf("%d", sizeRead);
									//		printf("%d\n\n\n\n\n\n\n", sizeRead);
											name2 = (char*)malloc((sizeRead + 1) * sizeof(char));
											read(fileNumber, name2, sizeRead);
											name2[sizeRead] = '\0';
											//memset(bpb4, 0, sizeof(bpb4));
											offsetSize = 0;
											count3++;
											fwrite(name2, sizeof(char), sizeRead, stdout);

										}
										if (direct.DIR_Name[0] == 0x0) {
											//printf("%s", "HELLO6");
											count3++;
											break;
										}
										//		read(fileNumber, read1, 32);

									//	printf("%s", name2);
										//	break;
										free(name2);
										memcpy(&direct, name, sizeof(struct DirectoryEntry));
										//	one = strchr(direct.DIR_Name, ' ');
										//	if (one != NULL && 11 > (one - direct.DIR_Name)) {
										//		direct.DIR_Name[(one - direct.DIR_Name)] = '\0';
										//	}


										//	else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
										//		count2++;
										//	}
										 /*else {
												if (strcmp(direct.DIR_Name, ".") == 0) {
													//count3++;
												}
												if (count3 == 2) {
													//break;
												}

											//	printf("%s", direct.DIR_Name);
												//printf(read1);
												//printf("%d\n", direct.DIR_FileSize);
												//printf("%d\n", offset2 + count2 * 32);
												//printf("%d\n", offsetCluster = (512 * 32) + ((direct.DIR_FstClusLO * 4)));

												count2++;
											}*/

									}
									if (*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8 || count3 > 0) {
										//	printf("it is4 =%d\n", offsetCluster);
										count3++;
										break;
									}

									//printf("%d\n", count3);
								}
								count3++;
								break;
							}
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

				if (srlCheck != 1) {
					fprintf(stderr, "%s is not opened or not opened as a writable file\n", direct.DIR_Name);
				}

				printf("read entered\n");
			}
		}
		else if (strcmp(command, "write") == 0) {
			int srlCheck = 0;
			int count = 0;
			int count2 = 0;
			int count3 = 0;
			int total = 0;
			int offset2 = 0;
			int offset3 = 0;
			int offset4 = 0;
			int offsetCluster2 = 0;
			int fileSize = 0;
			int offsetSize = 0;
			int sizeRead = 0;
			int k = 0;
			int wannabe = 0x0FFFFFF8;
			unsigned char string[100000];
			char keep[4];
			char keep3[4];
			char keep4[4];
			char keep2[4];
			char keep5[4];
			unsigned char  name2[100000];
			struct DirectoryEntry direct;
			unsigned char *one;
			unsigned int newCap = capture2;
			unsigned int newCap2 = capture2;
			char pointless[100];
			if (temp != ' ') {

			}
			else {
				scanf("%s %s %s \"%[^\"]%[^\n]", cmd2, keep3, keep4, string, pointless);

				offsetSize = atoi(keep3);
				sizeRead = atoi(keep4);
				//printf("%d\n", offsetSize);
			//	printf("%s\n", string);
				//break;
					/*	if (strcmp("", cmd2) == 0) {
							printf("%s\n", "File Does Not exist");
							memset(cmd2, 0, sizeof(cmd2));
						}*/
						//printf("%s\n", cmd2);

					//	break;
				while (1)
				{
					//	printf("%d\n", offsetSize);
					//	printf("%d\n", sizeRead);
						//	printf("%s", "HELLO1");
					offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap2 * 4));
					offset = (((newCap2 - 2)*capture4) + fileSec)*capture3;
					lseek(fileNumber, offsetCluster, SEEK_SET);
					read(fileNumber, keep2, 4);
					newCap2 = *(int*)keep2;
					count = 0;
					//printf("stuck");
					while (1)
					{
						//printf("%d\n", offsetSize);
						//printf("%d\n", sizeRead);
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
						if (count * 32 >= 512) {
							break;
						}
						if (direct.DIR_Name[0] == 0x0 || (rootDir == 0 && strcmp("..", cmd2) == 0)) {
							//	printf("%s", "HELLO4");
							printf("%s\n", "File doesnt exist");
							count3++;
							break;
						}


						else if (direct.DIR_Attr == 0x10 && strcmp(direct.DIR_Name, cmd2) == 0) {
							//	printf("%s", "HELLO4");
							printf("%s\n", "This is a Directory");
							count3++;
							break;
						}
						else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
							//	printf("%s", "HELLO5");
							count++;
						}
						else if (strcmp(direct.DIR_Name, cmd2) == 0 || strcmp("", cmd2) == 0 || ((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {

							int indexFinder = SearchOpenFiles(direct.DIR_Name);
							if (indexFinder < MAX_OPEN_FILES && (openFiles.files[indexFinder].DIR_Mode == 2 || openFiles.files[indexFinder].DIR_Mode == 3 || openFiles.files[indexFinder].DIR_Mode == 4)) {

								srlCheck = 1;

								//printf("%s\n",cmd2);

								//		printf("%d\n", direct.DIR_FstClusHI);
								//		printf("%d\n", direct.DIR_FstClusLO);

								fileSize = direct.DIR_FileSize;
								//break;
								//printf("%s", "HELLO5");

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
									newCap = direct.DIR_FstClusLO + (direct.DIR_FstClusHI << 16);
								}
								while (1)
								{
									if (fileSize < offsetSize) {
										count3++;
										break;
									}
									if (fileSize < offsetSize + sizeRead) {
										//count3++;
										//break;

									//	printf("%s", "HELLO7");
										//break;
										int bob = offsetSize + sizeRead;
										lseek(fileNumber, offset + (count * 32) + 28, SEEK_SET);
										write(fileNumber, &bob, 4);
										//break;
									}
									offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
									offset2 = (((newCap - 2)*capture4) + fileSec)*capture3;
									//offsetCluster = offsetCluster % capture3;
									lseek(fileNumber, offsetCluster, SEEK_SET);
									read(fileNumber, keep, 4);
									//	printf(" =%d\n", offsetCluster);

										//printf("it is =%d\n", offset2);
										//break;

								//	write(fileNumber, string, strlen(string));

									newCap = *(int*)keep;



									count3 = 0;

									//if (strlen(string) < sizeRead) {
									//	sizeRead = strlen(string);
										//printf("%s\n", "lllllllllllllllllllllllllllllllllllllllllll");
									//}
									//   printf("it is the count=%d\n", total);
										//printf("%d\n", offsetSize);
									//	printf("%d\n", sizeRead);
									//printf("%s", "HELLO19");
									count2 = 0;
									if (strlen(string) < sizeRead) {
										//	printf("%s", "HELLO5");
										for (k = strlen(string); k < sizeRead; k++) {
											//	printf("it is the count=%d\n", strlen(string));
											string[k] = (char)0;

										}
									}
									if (bob.BPB_BytsPerSe < offsetSize) {
										offsetSize = offsetSize - 512;
										//	printf("%s\n", "lllllllllllllllllllllllllllllllllllllllllll");
									}
									else {
										//printf("%s", "HELLO5");
										//	printf("%d\n", offset2);
										lseek(fileNumber, offset2 + offsetSize, SEEK_SET);

										//printf("%d", sizeRead);
										if (bob.BPB_BytsPerSe < sizeRead + offsetSize) {
											//name2 = (char*)malloc((513 - offsetSize) * sizeof(char));

											//printf("%s", "HELLO7");
											sizeRead = sizeRead - (bob.BPB_BytsPerSe - offsetSize);
											memcpy(name2, &string[offset3], (bob.BPB_BytsPerSe - offsetSize));
											offset3 += (bob.BPB_BytsPerSe - offsetSize);
											//name2[512 - offsetSize] = '\0';
										//	printf("%s\n",name2);
											write(fileNumber, name2, (512 - offsetSize));
											//	memset(bpb4, 0, sizeof(bpb4));
											offsetSize = 0;
											//printf("%s\n", "lllllllllllllllllllllllllllllllllllllllllll");
										//		printf("%s", "HELLO5");

										}
										else
										{

											//	printf("%s", "HELLO6");
											//	printf("%d", sizeRead);
												//printf("%d", sizeRead);
										//	name2 = (char*)malloc((sizeRead + 1) * sizeof(char));

											memcpy(name2, &string[offset3], sizeRead);
											offset3 += (sizeRead);

											//printf("%s\n", name2);
											//name2[sizeRead] = '\0';
											//printf("%s\n", name2); printf("%s\n", name2);
										//	printf("%s\n",name2);
											write(fileNumber, name2, sizeRead);

											read(fileNumber, name2, sizeRead);
											sizeRead = 0;
											//	printf(name2);

												//memset(bpb4, 0, sizeof(bpb4));
											offsetSize = 0;
											count3++;

										}
										//printf("%s", "HELLO7");
										if (direct.DIR_Name[0] == 0x0) {
											//printf("%s", "HELLO6");
											count3++;
											break;
										}
										//		read(fileNumber, read1, 32);

									//	printf("%s", name2);
										//	break;
										//free(name2);
										memcpy(&direct, name, sizeof(struct DirectoryEntry));


									}
									if (sizeRead != 0 && (*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8)) {
										//printf("%s", "HELLO");
										for (k = 0; 1; k++) {
											offsetCluster2 = (bob.BPB_BytsPerSe * 32) + (((*(int*)bpb6 + k) * 4));

											lseek(fileNumber, offsetCluster2, SEEK_SET);
											read(fileNumber, keep5, 4);
											if (*(int*)keep5 == 0x0) {
												lseek(fileNumber, offsetCluster2, SEEK_SET);
												write(fileNumber, &wannabe, 4);
												lseek(fileNumber, offsetCluster, SEEK_SET);
												write(fileNumber, &k, 4);
												newCap = k;
												//	break;
											}
										}
									}
									else if (*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8 || count3 > 0) {
										//	printf("it is4 =%d\n", offsetCluster);
										count3++;
										break;
									}

									//printf("%d\n", count3);
								}
								count3++;
								break;
							}

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

				//printf("read entered\n");
			}

			if (srlCheck != 1) {
				fprintf(stderr, "%s is not opened or not opened as a writable file\n", direct.DIR_Name);
			}

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

					if (direct.DIR_Name[0] == 0x0) {
						printf("file does not exist\n");
						count3++;
						break;
					}
					else if (count * 32 >= bob.BPB_BytsPerSe) {
						break;
					}
					else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
						count++;
					}
					else if (strcmp(direct.DIR_Name, cmd2) == 0) {
						printf("%d\n", direct.DIR_FileSize);
						count3++;
						break;
					}

					else {
						count++;
					}

				}
				if (*(int*)keep2 == 0x0FFFFFFF || *(int*)keep2 == 0x0FFFFFFE || *(int*)keep2 == 0x0FFFFFF8 || count3 != 0) {
					//	printf("it is4 =%d\n", offsetCluster);
					break;
				}
			}
		}
		else if (strcmp(command, "rm") == 0) {
			//printf("%s\n", "HELLO1");
			int count = 0;
			int count2 = 0;
			int count3 = 0;
			int total = 0;
			int offset2 = 0;
			char keep[4];
			char keep2[4];
			unsigned char stop[512];
			int free = 0;
			//printf("%s\n", "HELLO1");
			memset(stop, '\0', sizeof(stop));
			//memset(free, '\0', sizeof(free));
			char * name2;
			struct DirectoryEntry direct;
			struct DirectoryEntry empty;
			struct DirectoryEntry holding;
			unsigned char *one;
			unsigned int newCap = capture2;
			unsigned int newCap2 = capture2;
			unsigned char returning[32];
			//printf("%s\n", "HELLO1");
			if (temp != ' ') {

			}
			else {
				scanf("%s", cmd2);


				//printf("%d\n", sizeRead);
				//break
					/*	if (strcmp("", cmd2) == 0) {
							printf("%s\n", "File Does Not exist");
							memset(cmd2, 0, sizeof(cmd2));
						}*/
						//printf("%s\n", cmd2);

					//	break;
				while (1)
				{
					//	printf("%d\n", offsetSize);
					//	printf("%d\n", sizeRead);
					//printf("%s\n", "HELLO2");
					offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap2 * 4));
					offset = (((newCap2 - 2)*capture4) + fileSec)*capture3;
					lseek(fileNumber, offsetCluster, SEEK_SET);
					read(fileNumber, keep2, 4);
					newCap2 = *(int*)keep2;
					count = 0;
					//printf("stuck");
					while (1)
					{
						//	printf("%d\n", offsetSize);
						//	printf("%d\n", sizeRead);
						//	printf("stuck2");
						//	offsetCluster = (512 * 32) + (capture2 * 4);

						lseek(fileNumber, offset + (count * 32), SEEK_SET);
						read(fileNumber, name, 32);
						memcpy(&direct, name, sizeof(struct DirectoryEntry));
						one = strchr(direct.DIR_Name, ' ');
						if (one != NULL && 11 > (one - direct.DIR_Name)) {
							direct.DIR_Name[(one - direct.DIR_Name)] = '\0';

						}
						if (count * 32 >= bob.BPB_BytsPerSe) {
							break;
						}
						if (direct.DIR_Name[0] == 0x0 || (rootDir == 0 && strcmp("..", cmd2) == 0)) {
							//	printf("%s", "HELLO4");
							printf("%s \n", "File doesnt exist");
							count3++;
							break;
						}


						else if (direct.DIR_Attr == 0x10 && strcmp(direct.DIR_Name, cmd2) == 0) {
							//	printf("%s", "HELLO4");
							printf("%s\n", "This is a Directory");
							count3++;
							break;
						}
						else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
							//	printf("%s", "HELLO5");
							count++;
						}
						else if (strcmp(direct.DIR_Name, cmd2) == 0 || strcmp("", cmd2) == 0 || ((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
							//printf("%s\n",cmd2);


							//count3++;
							//break;

							lseek(fileNumber, offset + ((count + 1) * 32), SEEK_SET);
							if (count * 32 < bob.BPB_BytsPerSe) {
								read(fileNumber, name, 32);
								memcpy(&holding, name, sizeof(struct DirectoryEntry));
								if (holding.DIR_Name[0] == 0x0) {
									empty.DIR_Name[0] = 0x0;
								}
								else {
									empty.DIR_Name[0] = 0xE5;
								}
							}
							else {
								empty.DIR_Name[0] = 0x0;
							}
							lseek(fileNumber, offset + ((count) * 32), SEEK_SET);
							memcpy(returning, &empty, sizeof(struct DirectoryEntry));
							write(fileNumber, returning, 32);
							//memcpy()

							//break;
				//printf("%s", "HELLO5");
							//count3++;


							//
							newCap = direct.DIR_FstClusLO + (direct.DIR_FstClusHI << 16);

							while (1)
							{
								//printf("%s\n", "HELLO4wewe");
								if (newCap != 0) {
									offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
									offset2 = (((newCap - 2)*capture4) + fileSec)*capture3;
									//offsetCluster = offsetCluster % capture3;
									lseek(fileNumber, offsetCluster, SEEK_SET);
									read(fileNumber, keep, 4);
									newCap = *(int*)keep;

									lseek(fileNumber, offsetCluster, SEEK_SET);
									write(fileNumber, &free, 4);
									lseek(fileNumber, offset2, SEEK_SET);
									write(fileNumber, stop, 512);

								}
								else {
									count3++;
									break;

								}





								if (*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8 || count3 > 0) {
									//	printf("it is4 =%d\n", offsetCluster);
									count3++;
									break;
								}
							}
							//printf("%d\n", count3);

							count3++;
							break;

						}
						//count++;
						else {
							count++;
							//printf("%s %d\n", "HELLO4",count3);
						}
						//	printf("%d\n", count3);

					}
					//printf("%d\n", count3);
					if (*(int*)keep2 == 0x0FFFFFFF || *(int*)keep2 == 0x0FFFFFFE || *(int*)keep2 == 0x0FFFFFF8 || count3 != 0) {
						//printf("it is4 =%d\n", offsetCluster);
						break;
					}
				}

				printf("remove entered\n");
			}

		}
		else if (strcmp(command, "cp") == 0) {
			int count = 0;
			int count2 = 0;
			int count3 = 0;
			int count4 = 0;
			int count5 = 0;
			int k = 0;
			int total = 0;
			int offset2 = 0;
			int offset3 = 0;
			int offsetCluster2 = 0;
			int dont = 0;
			int wannabe = 0x0FFFFFF8;
			char keep[4];
			char keep2[4];
			char keep5[4];
			char keep6[32];
			unsigned char stop[10000];
			char picks[1000];
			int free = 0;
			//	printf("%s\n", "HELLO1");
			memset(stop, '\0', sizeof(stop));
			//memset(free, '\0', sizeof(free));
			char * name2;
			struct DirectoryEntry direct;
			struct DirectoryEntry empty;
			struct DirectoryEntry create;
			struct DirectoryEntry holding;
			unsigned char *one;
			unsigned int newCap = capture2;
			unsigned int newCap2 = capture2;
			unsigned int newCap3 = 0;
			int directoryOffset = 0;
			unsigned char returning[32];
			unsigned int firstCLUSter = 0;
			unsigned int fileSize = 0;
			int safe = 0;

			//printf("%s\n", "HELLO1");
			if (temp != ' ') {

			}
			else {
				scanf("%s %s", cmd2, picks);
				//	printf("%s\n", "HELLO2 1");
				while (1)
				{
					//printf("%s\n", "HELLO3 1");
					//	printf("%s", "HELLO1");
					offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap2 * 4));
					offset2 = (((newCap2 - 2)*capture4) + fileSec)*capture3;
					lseek(fileNumber, offsetCluster, SEEK_SET);
					read(fileNumber, keep2, 4);
					newCap2 = *(int*)keep2;
					count = 0;
					//	printf("stuck");s
					while (1)
					{
						//printf("%d\n", count);
						//	printf("%s", "stuck1\n");
						//	printf("stuck2");

						//	offsetCluster = (bob.BPB_BytsPerSe * 32) + (capture2 * 4);
						lseek(fileNumber, offset2 + (count * 32), SEEK_SET);
						read(fileNumber, name, 32);
						memcpy(&direct, name, sizeof(struct DirectoryEntry));
						//printf("%s2\n", direct.DIR_Name);
						one = strchr(direct.DIR_Name, ' ');
						if (one != NULL && 11 > (one - direct.DIR_Name)) {
							direct.DIR_Name[(one - direct.DIR_Name)] = '\0';

						}
						if (count * 32 >= bob.BPB_BytsPerSe) {
							//printf("stuck2\n");
							break;
						}
						if ((direct.DIR_Name[0] == 0x0) || (rootDir == 0 && strcmp("..", picks) == 0)) {
							//printf("stuck4\n");
						//	printf("%s\n", "HELLO4 1");
							//printf("%s\n", "Directory doesnt exist");
							count3++;
							break;
						}


						else if (direct.DIR_Attr == 0x20 && strcmp(direct.DIR_Name, picks) == 0) {
							//printf("%s", "HELLO4 1");
							printf("%s\n", "This is a file");
							dont = 1;
							count3++;
							break;
						}
						else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
							//printf("%s", "HELLO5");
							//printf("stuck5 1\n");
							count++;
						}
						else if (strcmp(direct.DIR_Name, picks) == 0 || strcmp("", picks) == 0 || ((rootDir - 1) == 0 && strcmp("..", picks) == 0) || ((rootDir) == 0 && strcmp(".", picks) == 0)) {
							//printf("%s\n",cmd2);
							//printf("%s\n", "HELLO5 1");
							safe = 1;
							//		printf("%d\n", direct.DIR_FstClusHI);
							//		printf("%d\n", direct.DIR_FstClusLO);

						//	printf("%s", "HELLO1");
									//break;
							//printf("%s", "HELLO5");
							if (strcmp("", cmd2) == 0) {
								//		printf("%s", "HELLO4");
								newCap = capture2;
							}
							else if (((rootDir - 1) == 0 && strcmp("..", picks) == 0) || ((rootDir) == 0 && strcmp(".", picks) == 0)) {
								newCap = *(int*)bpb6;
								//printf("%s", "HELLO3");
							}
							else
							{
								//printf("%s", "HELLO4");
								newCap = direct.DIR_FstClusLO + (direct.DIR_FstClusHI << 16);
							}
							while (1)
							{
								//
							//	printf("%s\n", "HELLO6 1");
								//	break;
								offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
								offset2 = (((newCap - 2)*capture4) + fileSec)*capture3;
								//offsetCluster = offsetCluster % capture3;
								lseek(fileNumber, offsetCluster, SEEK_SET);
								read(fileNumber, keep, 4);
								//	printf("it is off =%d\n", offsetCluster);

								//	printf("it is =%d\n", capture3);
									//break;



								newCap = *(int*)keep;


								//printf("keeper %d\n",*(int*)keep);


								//printf("it is the count=%d\n", total);
								count3 = 0;

								count2 = 0;
								while (1)
								{
									//printf("%s\n", "HELLO7 1");
									//	printf("%s", "HELLO5");
										//printf("%d\n", offset2);
									lseek(fileNumber, offset2 + (count2 * 32), SEEK_SET);

									read(fileNumber, name, 32);
									memcpy(&direct, name, sizeof(struct DirectoryEntry));
									one = strchr(direct.DIR_Name, ' ');
									if (count2 * 32 >= bob.BPB_BytsPerSe) {
										break;
									}
									if (one != NULL && 11 > (one - direct.DIR_Name)) {
										direct.DIR_Name[(one - direct.DIR_Name)] = '\0';
									}

									if (direct.DIR_Name[0] == 0x0 && count2 * 32 < bob.BPB_BytsPerSe) {
										directoryOffset = offset2 + (count2 * 32);
										count3++;
										count4++;
										break;
									}
									else if (direct.DIR_Name[0] == 0xE5) {
										directoryOffset = offset2 + (count2 * 32);
										count3++;
										count4++;
										break;
										//count2++;
									}
									else {
										if (strcmp(direct.DIR_Name, ".") == 0) {
											//count3++;
										}
										if (count3 == 2) {
											//break;
										}

										count2++;
									}

								}
								if (count4 != 0) {

									count4 = 0; break;
								}
								else if (*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8) {
									//printf("it is4 1 =%d\n", offsetCluster);
									for (k = 0; 1; k++) {
										offsetCluster2 = (bob.BPB_BytsPerSe * 32) + (((*(int*)bpb6 + k) * 4));
										lseek(fileNumber, offsetCluster2, SEEK_SET);
										read(fileNumber, keep5, 4);
										if (*(int*)keep5 == 0) {
											//	printf("%s %d 1\n", "here", k);
											lseek(fileNumber, offsetCluster2, SEEK_SET);
											write(fileNumber, &wannabe, 4);
											lseek(fileNumber, offsetCluster, SEEK_SET);
											write(fileNumber, &k, 4);
											newCap = k;
											break;
											//cout3++;
										}
									}
									//break;
								}

								//printf("%d\n", count3);
							}
							count3++;
							break;

						}
						//count++;
						else {
							//printf("stuck6\n");
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
				count3 = 0;
				//printf("%d\n", directoryOffset);
				newCap = capture2;
				newCap2 = capture2;
				newCap3 = 0;
				if (safe == 0) {
					safe = -1;
				}
				while (1)
				{
					//	printf("%d\n", offsetSize);
					//	printf("%d\n", sizeRead);
					//printf("%s\n", "HELLO2 1");
					offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap2 * 4));
					offset = (((newCap2 - 2)*capture4) + fileSec)*capture3;
					lseek(fileNumber, offsetCluster, SEEK_SET);
					read(fileNumber, keep2, 4);
					newCap2 = *(int*)keep2;
					count = 0;
					//printf("stuck\n");
					while (1)
					{
						//	printf("%d\n", offsetSize);
						//	printf("%d\n", sizeRead);
					//	printf("%s\n", "HELLO3 1");
						//	printf("stuck2");

						//	offsetCluster = (512 * 32) + (capture2 * 4);

						lseek(fileNumber, offset + (count * 32), SEEK_SET);
						read(fileNumber, name, 32);
						memcpy(&direct, name, sizeof(struct DirectoryEntry));
						//printf("%s\n", direct.DIR_Name);
						one = strchr(direct.DIR_Name, ' ');
						if (one != NULL && 11 > (one - direct.DIR_Name)) {
							direct.DIR_Name[(one - direct.DIR_Name)] = '\0';

						}
						if (count * 32 >= bob.BPB_BytsPerSe) {
							break;
						}
						if (direct.DIR_Name[0] == 0x0 || (rootDir == 0 && strcmp("..", cmd2) == 0)) {
							//printf("%s", "HELLO4");
							dont = 2;
							printf("%s \n", "Could not find file");
							count3++;
							break;
						}


						else if (direct.DIR_Attr == 0x10 && strcmp(direct.DIR_Name, cmd2) == 0) {
							//printf("%s", "HELLO4 1");
							dont = 1;
							printf("%s\n", "This is a Directory");
							count3++;
							break;
						}
						else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
							//	printf("%s", "HELLO5");
							count++;
						}
						else if (strcmp(direct.DIR_Name, cmd2) == 0 || strcmp("", cmd2) == 0 || ((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
							////printf("%s\n",cmd2);
							//printf("%s\n", "HELLO333335 1");
							if (safe == 1) {
								//printf("%s\n", "HELLO80");
								safe = 2;
							}


							//		printf("%d\n", direct.DIR_FstClusHI);
							//		printf("%d\n", direct.DIR_FstClusLO);
							create.DIR_Attr = direct.DIR_Attr;
							strcpy(create.DIR_Name, direct.DIR_Name);
							create.DIR_FileSize = direct.DIR_FileSize;
							fileSize = direct.DIR_FileSize;
							lseek(fileNumber, offset + ((count + 1) * 32), SEEK_SET);
							if (count * 32 < bob.BPB_BytsPerSe) {
								read(fileNumber, name, 32);
								memcpy(&holding, name, sizeof(struct DirectoryEntry));
								if (holding.DIR_Name[0] == 0x0) {
									empty.DIR_Name[0] = 0x0;
								}
								else {
									empty.DIR_Name[0] = 0xE5;
								}
							}
							else {
								empty.DIR_Name[0] = 0x0;
							}
							//	lseek(fileNumber, offset + ((count) * 32), SEEK_SET);
							//	memcpy(returning, &empty, sizeof(struct DirectoryEntry));
								//write(fileNumber, returning, 32);
								//memcpy()
							//printf("%s\n", "HELLO7");
							//break;
				//printf("%s", "HELLO51");


							if (strcmp("", cmd2) == 0) {
								//printf("%s\n", "HELLO54");
								newCap = capture2;
							}
							else if (((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
								newCap = *(int*)bpb6;
								//printf("%s\n", "HELLO32");
							}
							else
							{
								//	printf("%s\n", "HELLO40");
								newCap = direct.DIR_FstClusLO;
							}
							offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
							while (1)
							{
								//	printf("%s\n", "HELLO8 1");
									//printf("%s\n", "HELLO8");
									//printf("%s\n", "HELLO8");
								if (newCap3 != 0) {
									//	printf("%s\n", "HELLO9");

									offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
									offset2 = (((newCap - 2)*capture4) + fileSec)*capture3;
									offsetCluster2 = (bob.BPB_BytsPerSe * 32) + ((newCap3 * 4));
									offset3 = (((newCap3 - 2)*capture4) + fileSec)*capture3;
									//offsetCluster = offsetCluster % capture3;
									lseek(fileNumber, offsetCluster, SEEK_SET);
									read(fileNumber, keep, 4);
									newCap = *(int*)keep;
									//	printf("%d\n", offsetCluster);
									lseek(fileNumber, offset2, SEEK_SET);
									read(fileNumber, stop, bob.BPB_BytsPerSe);
									//	printf("%d this is not a life",*(int*)keep);
										//fwrite(stop, sizeof(char), 512, stdout);
									lseek(fileNumber, offset3, SEEK_SET);
									write(fileNumber, stop, bob.BPB_BytsPerSe);
									if (*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8) {
										count3++;
										break;
									}
									fileSize = fileSize - 512;
								}
								if (fileSize <= 0) {
									count3++;
									break;
								}
								for (k = 0; 1; k++) {
									//printf("%s\n", "HELLO8");

									offsetCluster2 = (bob.BPB_BytsPerSe * 32) + (((*(int*)bpb6 + k) * 4));
									offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap3 * 4));
									lseek(fileNumber, offsetCluster2, SEEK_SET);
									read(fileNumber, keep5, 4);
									if (*(int*)keep5 == 0x0) {
										lseek(fileNumber, offsetCluster2, SEEK_SET);
										write(fileNumber, &wannabe, 4);
										if (newCap3 == 0) {
											firstCLUSter = k;
										}
										else {
											lseek(fileNumber, offsetCluster, SEEK_SET);
											write(fileNumber, &k, 4);

										}
										newCap3 = k;
										break;
										//cout3++;
									}
								}





								//if ((*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8) || count3 > 0 ) {
									//	printf("it is4 =%d\n", count3);
									//count3++;
								//	break;
							//	}
							}
							//printf("%d\n", count3);

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
				count3 = 0;
				//printf("%d\n", directoryOffset);
				newCap = capture2;
				newCap2 = capture2;
				count = 0;
				count2 = 0;


				if (safe == -1 && dont == 0) {
					newCap = capture2;
					while (1)
					{
						//
					//	printf("%s\n", "HELLO6	23123");
						//	break;
						offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
						offset2 = (((newCap - 2)*capture4) + fileSec)*capture3;
						//offsetCluster = offsetCluster % capture3;
						lseek(fileNumber, offsetCluster, SEEK_SET);
						read(fileNumber, keep, 4);
						//	printf("it is =%d\n", offsetCluster);

							//printf("it is =%d\n", capture3);
							//break;



						newCap = *(int*)keep;





						//printf("it is the count=%d\n", total);
						count3 = 0;

						count2 = 0;
						while (1)
						{
							//printf("%s\n", "HELLO7");
							//	printf("%s", "HELLO5");
								//printf("%d\n", offset2);
							lseek(fileNumber, offset2 + (count2 * 32), SEEK_SET);

							read(fileNumber, name, 32);
							memcpy(&direct, name, sizeof(struct DirectoryEntry));
							one = strchr(direct.DIR_Name, ' ');
							if (count2 * 32 >= bob.BPB_BytsPerSe) {
								break;
							}
							if (one != NULL && 11 > (one - direct.DIR_Name)) {
								direct.DIR_Name[(one - direct.DIR_Name)] = '\0';
							}

							if (direct.DIR_Name[0] == 0x0 && count2 * 32 < bob.BPB_BytsPerSe) {
								directoryOffset = offset2 + (count2 * 32);
								count4++;
								break;
							}
							else if (direct.DIR_Name[0] == 0xE5) {
								directoryOffset = offset2 + (count2 * 32);
								count4++;
								break;
								//count2++;
							}
							else {
								if (strcmp(direct.DIR_Name, ".") == 0) {
									//count3++;
								}
								if (count3 == 2) {
									//break;
								}

								count2++;
							}

						}
						if (count4 != 0) {

							count4 = 0; break;
						}
						else if (*(int*)keep == 0x0FFFFFFF || *(int*)keep == 0x0FFFFFFE || *(int*)keep == 0x0FFFFFF8) {
							//	printf("it is4 =%d\n", offsetCluster);
							for (k = 0; 1; k++) {
								offsetCluster2 = (bob.BPB_BytsPerSe * 32) + (((*(int*)bpb6 + k) * 4));
								//printf("%s %d\n", "HELLO1", k);
								lseek(fileNumber, offsetCluster2, SEEK_SET);
								read(fileNumber, keep5, 4);
								if (*(int*)keep5 == 0) {
									//	printf("%s %d\n", "HELLO2bb", k);
									lseek(fileNumber, offsetCluster2, SEEK_SET);
									write(fileNumber, &wannabe, 4);
									lseek(fileNumber, offsetCluster, SEEK_SET);
									directoryOffset = (((k - 2)*capture4) + fileSec)*capture3;
									write(fileNumber, &k, 4);
									newCap = k;
									count3++;
									break;

								}
							}
							break;
						}


						//printf("%d\n", count3);
					}
					//printf("cp entered\n");
					strncpy(create.DIR_Name, picks, 8);
					unsigned short high = firstCLUSter >> 16;
					unsigned short low = ((firstCLUSter << 16) >> 16);
					offset = (((low - 2)*capture4) + fileSec)*capture3;
					//printf("%d\n", offset);
					//printf("%d\n", low);
					create.DIR_FstClusHI = high;
					create.DIR_FstClusLO = low;
					lseek(fileNumber, directoryOffset, SEEK_SET);

					memcpy(keep6, &create, sizeof(struct DirectoryEntry));
					//memset(keep6);
					write(fileNumber, keep6, 32);
				}
				else if (safe == 2 && dont == 0) {
					//printf("%s\n", "HELLO6ssssssssssssssss	23123");
					//printf("cp entered\n");
					unsigned short high = firstCLUSter >> 16;
					unsigned short low = ((firstCLUSter << 16) >> 16);
					offset = (((low - 2)*capture4) + fileSec)*capture3;
					//printf("%d\n", offset);
					//printf("%d\n", low);
					create.DIR_FstClusHI = high;
					create.DIR_FstClusLO = low;
					lseek(fileNumber, directoryOffset, SEEK_SET);
					memcpy(keep6, &create, sizeof(struct DirectoryEntry));
					//memset(keep6);
					write(fileNumber, keep6, 32);
				}
				//		printf("cp entered\n");

			}
		}
		else if (strcmp(command, "exit") == 0)
			printf("exiting program. Good Bye\n");

		else
			printf("invalid command, try again...\n");


		memset(cmd2, 0, sizeof(cmd2));
	}
	close(fileNumber);
	rootDir = 0;
	capture2 = *(int*)bpb6;
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
/*int clusterFromPath(char * name) {
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


	return 0;
}*/

void PrintOpenFiles() {
	int srl;
	for (srl = 0; srl < openFiles.count; srl++) {
		printf("%s\n", openFiles.files[srl].DIR_Name);
	}
}

int GetOpenFileSlot() {
	int rValue = 0;
	while (rValue < MAX_OPEN_FILES) {
		if (openFiles.files[rValue].DIR_Name[0] == '\0') {
			return rValue;
		}
		rValue++;
	}
	fprintf(stderr, "Already opened max number of files\n");
	return 64;
}

int SearchOpenFiles(char * fname) {
	int rValue = 0;
	while (rValue < MAX_OPEN_FILES) {
		if (strcmp(openFiles.files[rValue].DIR_Name, fname) == 0) {
			return rValue;
		}
		rValue++;
	}
	return rValue;
}
