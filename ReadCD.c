#include <LIBDS.H>
#include <STRINGS.H>
#include <SYS/TYPES.H>
#include <STDLIB.H>
#include <STDIO.H>
#include <LIBSN.H>
#include <stdint.h>
#include <pcdrv.h>



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
uint8_t buffer[2048] = { 0 };
#endif


#ifdef _RELEASE_
int didInitDs = 0;
#else
int lastOpsVal = 0;

#endif

void ReadCDInit() {
	printf("\nReserving 1024KB (1,048,576 Bytes) RAM... \n");
    InitHeap3((void*)0x800F8000, 0x00100000);
    printf("Success!\n");
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

void cd_read_file(unsigned char* file_path, u_long** file) {
	#ifdef _RELEASE_
	u_char* file_path_raw;
	int* sectors_size;
	DslFILE* temp_file_info;
	sectors_size = malloc3(sizeof(int));
	temp_file_info = malloc3(sizeof(DslFILE));

	// Exit if libDs isn't initialized
	if(!didInitDs) {
		printf("LIBDS not initialized, run cdOpen() first\n");	
		return;
	}

	// Get raw file path
	file_path_raw = malloc3(4 + strlen(file_path));
	strcpy(file_path_raw, "\\");
	strcat(file_path_raw, file_path);
	strcat(file_path_raw, ";1");
	printf("Loading file from CD: %s\n", file_path_raw);

	// Search for file on disc
	DsSearchFile(temp_file_info, file_path_raw);

	// Read the file if it was found
	if(temp_file_info->size > 0) {
		printf("...file found\n");
		printf("...file size: %lu\n", temp_file_info->size);
		*sectors_size = temp_file_info->size + (SECTOR % temp_file_info->size);
		printf("...file buffer size needed: %d\n", *sectors_size);
		printf("...sectors needed: %d\n", (*sectors_size + SECTOR - 1) / SECTOR);
		*file = malloc3(*sectors_size + SECTOR);
		
		DsRead(&temp_file_info->pos, (*sectors_size + SECTOR - 1) / SECTOR, *file, DslModeSpeed);
		while(DsReadSync(NULL));
		printf("...file loaded!\n");
	} else {
		printf("...file not found\n");
	}

	// Clean up
	free3(file_path_raw);
	free3(sectors_size);
	free3(temp_file_info);
	#else
	//****** READING DATA FROM PCDRV
	char* file_path_raw;
	int handler = -1;
	file_path_raw = malloc3(7+ strlen(file_path));
	strcpy(file_path_raw,"assets/");
	strcat(file_path_raw,file_path);
	printf("Loading file from PCDRV: %s\n", file_path_raw);
	handler = PCopen( file_path_raw, FILEMODE_READONLY, 0);
	if(handler == -1){
		printf("File Not Found %s\n",file_path_raw);
		
	}
	else{
		printf("File Found!!! %s\n",file_path_raw);
		int fileSize = PClseek( handler, 0, 2 );
			if ( fileSize == -1 ){
				printf( "Couldn't seek to find the file size..." );
			} else {
				int returnToStart;
				printf( "File size 0x%x\n", fileSize );
				returnToStart = PClseek( handler, 0, 0 );
				if ( fileSize == -1 ){
                        printf( "Couldn't seek back to the start of the file..." );
                    } else {
						int newThingy = -1;
						
						char buf[16];
						PCread(handler, buf, fileSize);
                   		//PCread( handler, buf, fileSize );
						if ( newThingy == -1 ){
                            printf("Error reading the file!");
                        } else {
                            printf("Loaded File!!");
                        }

						
					}
			}


	}


	free3(file_path_raw);

	#endif
}