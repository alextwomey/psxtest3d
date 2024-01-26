#include <LIBDS.H>
#include <STRINGS.H>
#include <SYS/TYPES.H>
#include <STDLIB.H>
#include <STDIO.H>
#include <stdint.h>
#include "pcdrv.h"



//comment out of not full release for CD!!
//**********************************
// FOR RELEASE  ********************
//**********************************
//#define _RELEASE_


#ifdef _RELEASE_
#define SECTOR 2048
#else
#define FILEMODE_READONLY 0
#define FILEMODE_WRITEONLY 1
#define FILEMODE_READWRITE 2


#endif


#ifdef _RELEASE_
int didInitDs = 0;
#else
int lastOpsVal = 0;
void* dataBuffer; 
 
#endif

void ReadCDInit() {
	#ifdef _RELEASE_
	#else
	lastOpsVal = PCinit();
		
	#endif
}

void cd_open() {
	#ifdef _RELEASE_
	if(!didInitDs) {
		didInitDs = 1;
		DsInit();
	}
	#else

	#endif
}

void cd_close() {
	#ifdef _RELEASE_
	if(didInitDs) {
		didInitDs = 0;
		DsClose();
	}
	#else 

	#endif
}

void cd_read_file(unsigned char* filePath, u_long** file) {
	#ifdef _RELEASE_
	u_char* filePathRaw;
	int* sectorsSize;
	DslFILE* tempFileInfo;
	sectorsSize = malloc3(sizeof(int));
	tempFileInfo = malloc3(sizeof(DslFILE));

	// Exit if libDs isn't initialized
	if(!didInitDs) {
		printf("LIBDS not initialized, run cdOpen() first\n");	
		return;
	}

	// Get raw file path
	filePathRaw = malloc3(4 + strlen(filePath));
	strcpy(filePathRaw, "\\");
	strcat(filePathRaw, filePath);
	strcat(filePathRaw, ";1");
	printf("Loading file from CD: %s\n", filePathRaw);

	// Search for file on disc
	DsSearchFile(tempFileInfo, filePathRaw);

	// Read the file if it was found
	if(tempFileInfo->size > 0) {
		printf("...file found\n");
		printf("...file size: %lu\n", tempFileInfo->size);
		*sectorsSize = tempFileInfo->size + (SECTOR % tempFileInfo->size);
		printf("...file buffer size needed: %d\n", *sectorsSize);
		printf("...sectors needed: %d\n", (*sectorsSize + SECTOR - 1) / SECTOR);
		*file = malloc3(*sectorsSize + SECTOR);
		
		DsRead(&tempFileInfo->pos, (*sectorsSize + SECTOR - 1) / SECTOR, *file, DslModeSpeed);
		while(DsReadSync(NULL));
		printf("...file loaded!\n");
		printf("File INFO: %ld\n",file);
	} else {
		printf("...file not found\n");
	}

	// Clean up
	free3(filePathRaw);
	free3(sectorsSize);
	free3(tempFileInfo);
	#else
	//****** READING DATA FROM PCDRV
	char* filePathRaw;
	int handler = -1;
	filePathRaw = malloc3(7+ strlen(filePath));
	strcpy(filePathRaw,"assets/");
	strcat(filePathRaw,filePath);
	printf("Loading file from PCDRV: %s\n", filePathRaw);
	handler = PCopen( filePathRaw, FILEMODE_READONLY, 0);
	if(handler == -1){
		printf("File Not Found %s\n",filePathRaw);
	}
	else{
		printf("File Found!!! %s\n",filePathRaw);
		int fileSize = PClseek( handler, 0, 2 );
			if ( fileSize == -1 ){
				printf( "Couldn't seek to find the file size...\n" );
			} else {
				int returnToStart;
				
				returnToStart = PClseek( handler, 0, 0 );
				printf("return to start %d\n",returnToStart);
				if ( returnToStart == -1 ){
                        printf( "Couldn't seek back to the start of the file...\n" );
                    } else {
						dataBuffer = malloc3(fileSize);
						printf("File Size is: %d\n",fileSize);
						lastOpsVal = PCread(handler,dataBuffer, fileSize);
						printf("reeead\n");
                   		*file = dataBuffer;
						if ( lastOpsVal == -1 ){
                            printf("Error reading the file!\n");
                        } else {
                            printf("Loaded File!!\n");
                        }
					}
			}
				lastOpsVal = PCclose(handler);
				if(lastOpsVal == -1){
					printf("File Closing Error!!\n");
				}else{
					printf("File Closed!\n");
				}

	}


	free3(filePathRaw);
	free3(dataBuffer);
	#endif
}