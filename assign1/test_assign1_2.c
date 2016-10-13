#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"
#include "storage_mgr.c"

/* test name*/
char *testName;

/* test output files */
#define TESTPF "test_pagefile.bin"
//#define TESTPF "test_pagefile.txt"
/* prototypes for test functions */
static void testReadWriteOperations();

int main (void)
{

testReadWriteOperations();


return 0;

}


void testReadWriteOperations()
{

	SM_FileHandle fh;
	testName = "Read Wirte operations methods";
	int i;
	SM_PageHandle ph;
	
	ph = (SM_PageHandle) malloc(PAGE_SIZE);

	// create a new page file
	TEST_CHECK(createPageFile (TESTPF));
  	TEST_CHECK(openPageFile (TESTPF, &fh));
  	printf("created and opened file\n");
	
	//Reading first block using readBlock;	
	TEST_CHECK(readBlock(0,&fh,ph));	

	
	for (i=0; i < PAGE_SIZE; i++)
    	ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");
	printf("-------------------------------first block was empty\n");
  

	// change ph to be a string and write that one to disk
    	for (i=0; i < PAGE_SIZE; i++)
    	ph[i] = (i % 10) + '0';
   	TEST_CHECK(writeBlock (0, &fh, ph));
   	printf("-------------------------------writing first block\n");
	
	//CURRENT POSITION (GETBLOCKPOSITION)
	printf("-------------Current Position -----::%d-------\n",getBlockPos(&fh));	
	
	TEST_CHECK(writeBlock (1, &fh, ph));
   	printf("-------------------------------writing second block----------\n");
	printf("-------------Current Position -----::%d-------\n",getBlockPos(&fh));	

	TEST_CHECK(writeBlock (2, &fh, ph));
   	printf("-------------------------------writing third block-----------\n");
	printf("-------------Current Position -----::%d-------\n",getBlockPos(&fh));	
	
	//APPEND AN EMPTY BLOCK
	printf("------------------------------appending an empty block to file\n");
	TEST_CHECK(appendEmptyBlock(&fh));

	printf("-------------------------------Current Position -----::%d-------\n",getBlockPos(&fh));	
	//WRITING CURRENT BLOCK
  	printf("-------------------------------writing current block-----\n");
	TEST_CHECK(writeCurrentBlock(&fh,ph));
	printf("-------------Current Position -----::%d-------\n",getBlockPos(&fh));
	
	//Reading block
	printf("------------------------------Reading Second block--------\n");
	TEST_CHECK(readBlock(1,&fh,ph));
	for (i=0; i < PAGE_SIZE; i++)
    	ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  	//printf("reading first block\n");
	
	getBlockPos(&fh);
	
	//READING CURRENT BLOCK
	TEST_CHECK(readCurrentBlock(&fh,ph));
	for (i=0; i < PAGE_SIZE; i++)
    	ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  	printf("-------------------------------Reading current block---------------\n");
	
	//READING next BLOCK;
	//printf("------------------------------Reading Second block--------\n");
	TEST_CHECK(readNextBlock(&fh,ph));
	for (i=0; i < PAGE_SIZE; i++)
    	ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  	printf("-------------------------------Reading Next block---------------\n");
		
	//Reading Previous block
	TEST_CHECK(readPreviousBlock(&fh,ph));
	for (i=0; i < PAGE_SIZE; i++)
    	ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  	printf("-------------------------------Reading Previous block Done---------------\n");
	
	//Reading last block
	TEST_CHECK(readLastBlock(&fh,ph));
	for (i=0; i < PAGE_SIZE; i++)
    	ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  	printf("-------------------------------Reading last block Done---------------\n");
	
	// destroy new page file
	TEST_CHECK(destroyPageFile (TESTPF));  
  
  	TEST_DONE();
	
}


