#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
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
int clusterFromPath(char * name);
int main(int argc, char *argv[]) {


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
	unsigned int capture;
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
				offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap2 * 4));
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
									printf("O\n");
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
							count3++;
							//printf("%s", "HELLO3");
						}
						else
						{
							//	printf("%s", "HELLO4");
							capture2 = direct.DIR_FstClusLO + (direct.DIR_FstClusHI << 16);
							count3++;
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

			printf("creat entered\n");
		}
		else if (strcmp(command, "mkdir") == 0) {

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
			printf("%s\n", "HELLO1");
			if (temp != ' ') {

			}
			else {
				scanf("%s %s", cmd2, picks);
				printf("%s\n", "HELLO2");
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

						printf("%s %s\n", direct.DIR_Name, "dklsdk");
						one = strchr(direct.DIR_Name, ' ');
						if (one != NULL && 11 > (one - direct.DIR_Name)) {
							direct.DIR_Name[(one - direct.DIR_Name)] = '\0';

						}
						printf("%s\n", direct.DIR_Name);
						if ((direct.DIR_Name[0] == 0x0) || (rootDir == 0 && strcmp("..", picks) == 0)) {
							//	printf("%s", "HELLO4");
							printf("%s\n", "Directory does1nt exist");
							count3++;
							break;
						}


						else if (direct.DIR_Attr == 0x20 && strcmp(direct.DIR_Name, picks) == 0) {
							printf("%s", "HELLO43434334");
							directoryOffset2 = offset + (count * 32);
							file = 1;
							count3++;
							break;
						}
						else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
							printf("%s", "HELLO5");
							count++;
						}
						else if (strcmp(direct.DIR_Name, picks) == 0 || strcmp("", picks) == 0 || ((rootDir - 1) == 0 && strcmp("..", picks) == 0) || ((rootDir) == 0 && strcmp(".", picks) == 0)) {
							//printf("%s\n",cmd2);
							finnish = 1;
							printf("%s\n", "HELLO5");
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
										printf("%s %d\n", "HELLO1", k);
										lseek(fileNumber, offsetCluster2, SEEK_SET);
										read(fileNumber, keep5, 4);
										if (*(int*)keep5 == 0) {
											printf("%s %d\n", "HELLO2", k);
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
							printf("hekki");
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



				while (file != 1)
				{

					printf("%s\n", "HELLO3");
					//	printf("%s", "HELLO1");


					printf("%s\n", "HELLO3");
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
						printf("%s1\n", direct.DIR_Name);
						if ((direct.DIR_Name[0] == 0x0) || (rootDir == 0 && strcmp("..", cmd2) == 0)) {
							//	printf("%s", "HELLO4");
							printf("%s\n", "Directory doesnt exist");
							count3++;
							break;
						}



						else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
							//printf("%s", "HELLO5"); 
							count++;
						}
						else if (strcmp(direct.DIR_Name, cmd2) == 0 || strcmp("", cmd2) == 0 || ((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
							//printf("%s\n",cmd2);
							printf("%s\n", "HELLO5234");
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
								printf("%s\n", "HELLO7");

								direct.DIR_Name[(one - direct.DIR_Name)] = '\0';
							}

							if (holding.DIR_Name[0] == 0x0 || count * 32 > bob.BPB_BytsPerSe) {
								empty.DIR_Name[0] = 0x0;
								printf("%s\n", "HELLO7");
								directoryOffset2 = offset + (count * 32);
								count4++;
								break;
							}
							else {
								empty.DIR_Name[0] = 0xE5;
								printf("%s\n", "HELLO8");
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
















				if (file == 1) {

					strncpy(direct.DIR_Name, cmd2, 8);
					lseek(fileNumber, directoryOffset2, SEEK_SET);
					memcpy(returning, &direct.DIR_Name, sizeof(struct DirectoryEntry));
					write(fileNumber, returning, 32);
				}
				else if (finnish >= 2) {
					lseek(fileNumber, directoryOffset2, SEEK_SET);
					read(fileNumber, name, 32);

					lseek(fileNumber, directoryOffset2, SEEK_SET);
					memcpy(returning, &empty, sizeof(struct DirectoryEntry));
					if (empty.DIR_Name[0] == 0xE5) {
						printf("%s\n", "HELLO60");
					}
					write(fileNumber, returning, 32);
					lseek(fileNumber, directoryOffset, SEEK_SET);
					write(fileNumber, name, 32);
				}
				printf("mv entered %d\n", directoryOffset);
				printf("mv entered %d\n", directoryOffset2);

			}
		}
		else if (strcmp(command, "open") == 0) {

			printf("open entered\n");
		}
		else if (strcmp(command, "close") == 0) {

			printf("close entered\n");
		}
		else if (strcmp(command, "read") == 0) {
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
							printf("%s %d %s\n", "Directory doesnt exist", rootDir, cmd2);
							count3++;
							break;
						}


						else if (direct.DIR_Attr == 0x10 && strcmp(direct.DIR_Name, cmd2) == 0) {
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
								newCap = direct.DIR_FstClusLO;
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

				printf("read entered\n");
			}
		}
		else if (strcmp(command, "write") == 0) {
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

			if (temp != ' ') {

			}
			else {
				scanf("%s %s %s %[^\n\r]", cmd2, keep3, keep4, string);

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
							printf("%s %d %s\n", "Directory doesnt exist", rootDir, cmd2);
							count3++;
							break;
						}


						else if (direct.DIR_Attr == 0x10 && strcmp(direct.DIR_Name, cmd2) == 0) {
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

							fileSize = direct.DIR_FileSize;
							//break;
							printf("%s", "HELLO5");

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
									printf("%d", write(fileNumber, &bob, 4));
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
								printf("%s", "HELLO19");
								count2 = 0;
								if (strlen(string) < sizeRead) {
									printf("%s", "HELLO5");
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
										//	name2 = (char*)malloc((513 - offsetSize) * sizeof(char));

											//printf("%s", "HELLO7");
										sizeRead = sizeRead - (bob.BPB_BytsPerSe - offsetSize);
										memcpy(name2, &string[offset3], (512 - offsetSize));
										offset3 += (bob.BPB_BytsPerSe - offsetSize);
										//name2[512 - offsetSize] = '\0';
									//	printf("%s\n",name2);
										printf("%d\n", write(fileNumber, name2, (512 - offsetSize)));
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

										printf("%s\n", name2);
										//name2[sizeRead] = '\0';
										//printf("%s\n", name2); printf("%s\n", name2);
									//	printf("%s\n",name2);
										printf("%d\n", write(fileNumber, name2, sizeRead));

										read(fileNumber, name2, sizeRead);
										sizeRead = 0;
										//	printf(name2);

											//memset(bpb4, 0, sizeof(bpb4));
										offsetSize = 0;
										count3++;

									}
									printf("%s", "HELLO7");
									if (direct.DIR_Name[0] == 0x0) {
										printf("%s", "HELLO6");
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
									printf("%s", "HELLO");
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



					lseek(fileNumber, offset2 + (count * 32), SEEK_SET);
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
			printf("%s\n", "HELLO1");
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
						printf("%s\n", "HELLO3");
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
							printf("%s %d %s\n", "File doesnt exist", rootDir, cmd2);
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

							//		printf("%d\n", direct.DIR_FstClusHI);
							//		printf("%d\n", direct.DIR_FstClusLO);

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


							if (strcmp("", cmd2) == 0) {
								printf("%s\n", "HELLO5");
								newCap = capture2;
							}
							else if (((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
								newCap = *(int*)bpb6;
								//printf("%s\n", "HELLO3");
							}
							else
							{
								//printf("%s\n", "HELLO4");
								newCap = direct.DIR_FstClusLO;
							}
							while (1)
							{

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
			unsigned int firstCLUSter;
			unsigned int fileSize;
			int safe = 0;
			printf("%s\n", "HELLO1");
			if (temp != ' ') {

			}
			else {
				scanf("%s %s", cmd2, picks);
				printf("%s\n", "HELLO2");
				while (1)
				{
					printf("%s\n", "HELLO3");
					//	printf("%s", "HELLO1");
					offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap2 * 4));
					offset2 = (((newCap2 - 2)*capture4) + fileSec)*capture3;
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
						//	printf("%s\n", direct.DIR_Name);
						one = strchr(direct.DIR_Name, ' ');
						if (one != NULL && 11 > (one - direct.DIR_Name)) {
							direct.DIR_Name[(one - direct.DIR_Name)] = '\0';

						}

						if ((direct.DIR_Name[0] == 0x0) || (rootDir == 0 && strcmp("..", picks) == 0)) {
							//	printf("%s", "HELLO4");
							printf("%s\n", "Directory doesnt exist");
							count3++;
							break;
						}


						else if (direct.DIR_Attr == 0x20 && strcmp(direct.DIR_Name, picks) == 0) {
							//	printf("%s", "HELLO4");
							printf("%s\n", "This is a file");
							count3++;
							break;
						}
						else if (direct.DIR_Name[0] == 0xE5 || direct.DIR_Attr == 0x0F) {
							//printf("%s", "HELLO5");
							count++;
						}
						else if (strcmp(direct.DIR_Name, picks) == 0 || strcmp("", picks) == 0 || ((rootDir - 1) == 0 && strcmp("..", picks) == 0) || ((rootDir) == 0 && strcmp(".", picks) == 0)) {
							//printf("%s\n",cmd2);
							printf("%s\n", "HELLO5");
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
										printf("%s %d\n", "HELLO1", k);
										lseek(fileNumber, offsetCluster2, SEEK_SET);
										read(fileNumber, keep5, 4);
										if (*(int*)keep5 == 0) {
											printf("%s %d\n", "HELLO2", k);
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
				printf("%d\n", directoryOffset);
				newCap = capture2;
				newCap2 = capture2;
				newCap3 = 0;
				while (safe != 0)
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
						//printf("%s\n", "HELLO3");
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
							printf("%s %d %s\n", "Directory doesnt exist", rootDir, cmd2);
							count3++;
							break;
						}


						else if (direct.DIR_Attr == 0x10 && strcmp(direct.DIR_Name, cmd2) == 0) {
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
							printf("%s\n", "HELLO5");
							if (safe == 1) {
								printf("%s\n", "HELLO80");
								safe = 2;
							}
							else {
								count3++;
								break;
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
						//	printf("%s\n", "HELLO7");
							//break;
				//printf("%s", "HELLO5");


							if (strcmp("", cmd2) == 0) {
								printf("%s\n", "HELLO54");
								newCap = capture2;
							}
							else if (((rootDir - 1) == 0 && strcmp("..", cmd2) == 0) || ((rootDir) == 0 && strcmp(".", cmd2) == 0)) {
								newCap = *(int*)bpb6;
								printf("%s\n", "HELLO32");
							}
							else
							{
								printf("%s\n", "HELLO40");
								newCap = direct.DIR_FstClusLO;
							}
							offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
							while (1)
							{
								//printf("%s\n", "HELLO8");
								if (newCap3 != 0) {
									printf("%s\n", "HELLO9");

									offsetCluster = (bob.BPB_BytsPerSe * 32) + ((newCap * 4));
									offset2 = (((newCap - 2)*capture4) + fileSec)*capture3;
									offsetCluster2 = (bob.BPB_BytsPerSe * 32) + ((newCap3 * 4));
									offset3 = (((newCap3 - 2)*capture4) + fileSec)*capture3;
									//offsetCluster = offsetCluster % capture3;
									lseek(fileNumber, offsetCluster, SEEK_SET);
									read(fileNumber, keep, 4);
									newCap = *(int*)keep;
									printf("%d\n", offsetCluster);
									lseek(fileNumber, offset2, SEEK_SET);
									read(fileNumber, stop, bob.BPB_BytsPerSe);
									printf(stop);
									fwrite(stop, sizeof(char), 512, stdout);
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
				if (safe == 2) {
					printf("cp entered\n");
					unsigned short high = firstCLUSter >> 16;
					unsigned short low = ((firstCLUSter << 16) >> 16);
					offset = (((low - 2)*capture4) + fileSec)*capture3;
					printf("%d\n", offset);
					printf("%d\n", low);
					create.DIR_FstClusHI = high;
					create.DIR_FstClusLO = low;
					lseek(fileNumber, directoryOffset, SEEK_SET);
					memcpy(keep6, &create, sizeof(struct DirectoryEntry));
					//memset(keep6);
					write(fileNumber, keep6, 32);
				}
				printf("cp entered\n");

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
