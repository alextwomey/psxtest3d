#include <LIBCD.H>
#include <STRINGS.H>
#include <SYS/TYPES.H>
#include <STDLIB.H>
#include <STDIO.H>
#define SECTOR 2048

int didInitDs = 0;

void ReadCDInit() {
	printf("\nReserving 1024KB (1,048,576 Bytes) RAM... \n");
    InitHeap3((void*)0x800F8000, 0x00100000);
    printf("Success!\n");
}

void cd_open() {
	if(!didInitDs) {
		didInitDs = 1;
		DsInit();
	}
}

void cd_close() {
	if(didInitDs) {
		didInitDs = 0;
		DsClose();
	}
}

void initThatCd(void){
	int CdInit(void);
}

char *cd_read_file_test(unsigned char* file_path){
	CdlFILE filePos;
	int numsecs;
	char *buff;
	printf("about to search for file %s\n",file_path);
	if( CdSearchFile(&filePos, file_path)==NULL){
		printf("File %s not found \n",file_path);
	}else{
		printf("File %s found!!!!!!!\n",file_path);
		/* calculate number of sectors to read for the file */
    	numsecs = (filePos.size+2047)/2048;
		printf("Size of sectors to read: %d");
		/* allocate buffer for the file */
		buff = (char*)malloc( 2048*numsecs );
		
		/* set read target to the file */
		CdControl( CdlSetloc, (u_char*)&filePos, 0 );
		
		/* start read operation */
		CdRead( numsecs, (u_long*)buff, CdlModeSpeed );
		
		/* wait until the read operation is complete */
		CdReadSync( 0, 0 );
	}
	
}

void cd_read_file(unsigned char* file_path, u_long** file) {

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
	DsSearchFile(file_path_raw, temp_file_info);
	

	// Read the file if it was found
	if(temp_file_info->size > 0) {
		printf("...file found\n");
		printf("...file size: %lu\n", temp_file_info->size);
		*sectors_size = temp_file_info->size + (SECTOR % temp_file_info->size);
		printf("...file buffer size needed: %d\n", *sectors_size);
		printf("...sectors needed: %d\n", (*sectors_size + SECTOR - 1) / SECTOR);
		printf("Making pointer: ");
		*file = malloc3(*sectors_size + SECTOR);
		printf("File pointer made: ");
		DsRead(&temp_file_info->pos, (*sectors_size + SECTOR - 1) / SECTOR, *file, DslModeSpeed);
		printf("DsRead finished");
		while(DsReadSync(NULL));
		printf("...file loaded!\n");
	} else {
		printf("...file not found\n");
	}

	// Clean up
	free3(file_path_raw);
	free3(sectors_size);
	free3(temp_file_info);

}