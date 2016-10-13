#include "storage_mgr.h"
#include "dberror.c"
#include "dberror.h"
#include "stdio.h"
#include "unistd.h"

#define RC_NO_PRIVIOUS_BLOCK 5
#define RC_NO_NEXT_BLOCK 6

void initStorageManager(){

	printf("-------------------------------INSIDE INITSTORAGEMANAGER FUNCTION--------\n");
}
/*createPageFile takes fileName as argument and creates and empty character arry of size 4096 and writes to the file. If process completed successful function returns RC_OK*/
RC createPageFile(char *fileName)
{
	static int rc = RC_FILE_NOT_FOUND;
	FILE *fp ;
	fp = fopen(fileName,"r");  
	char *buf = (char *)calloc(1,PAGE_SIZE);
	int i = 0,d,c;
	int input;
	//scanf("%d", &input);
	if(buf != NULL && fp == NULL)
	{	
		//for(i=0;i<PAGE_SIZE;i++)
		//{
		//	buf[i] = '\0';
		//}
	
		printf("----------------------FILE WILL BE CREATED------------------- \n ");	
		fp = fopen(fileName,"wb");
		fwrite(buf,PAGE_SIZE,1,fp);
		rc = RC_OK;
		fclose(fp);	
	}
	
	else
	{
		d = destroyPageFile(fileName);
		printf("---------------------FILE IS EXISTED AND RECREATING FILE-------\n");		
		c = createPageFile(fileName);
		
		
	}
	printf("---------------RC--------%d",rc);
	return rc;
}

/*openPageFile function takes fileName and fileHandle as arguments and assigns the fileName to fHandle->fileName, 0 to fHandle->curPagePos, 1 to fHandle->totNumPages and returns RC_OK if operation is completed.*/

RC openPageFile (char *fileName, SM_FileHandle *fHandle)
{
	int rc;
	FILE *fp;
	fp = fopen(fileName,"rw");
	if(fp != NULL)
	{
		
		if(fHandle == NULL)
		{
			printf("----------------------FILE HANDLE IS NOT INITIALISED--------------\n");
			rc = RC_FILE_HANDLE_NOT_INIT;
		}
		else
		{
			fHandle->fileName = fileName;
			printf("---------------------FILE NAME IS : %s--------------------------------\n",fHandle->fileName);
			fseek(fp,0,SEEK_END);	
			fHandle->totalNumPages = ftell(fp)/PAGE_SIZE;
			printf("----------------------TOTAL PAGES: %d-----------------------------\n",fHandle->totalNumPages);
			fHandle->curPagePos = fseek(fp,0,SEEK_SET);
			printf("--------------------CURRENT POSITION IS: %d-------------------------\n",fHandle->curPagePos);
			fHandle->mgmtInfo = fp;
			rc = RC_OK;
		}
		fclose(fp);

	}
	else
	{
		printf("----------------------FILE NOT FOUND-------------------------\n");
		rc = RC_FILE_NOT_FOUND;
		
	}
	
	return rc;
	
			
}

/*closePageFile function takes fileHandle as argument and closes the file and returns RC_OK if operation is completed.*/

RC closePageFile (SM_FileHandle *fHandle)
{
	int rc;
	FILE *fp;
	if(fHandle != NULL)
	{
		fp = fopen(fHandle->fileName,"r");
		if(fp != NULL)
		{
			fclose(fp);
			rc = RC_OK; 
		}
		else
		{
			printf("----------------------FILE NOT FOUND-------------------------\n");
			rc = RC_FILE_NOT_FOUND;
		}
	}
	else
	{
		printf("----------------------FILE HANDLE IS NOT INITIALISED--------------\n");
		rc = RC_FILE_HANDLE_NOT_INIT;
	}

	return rc;
}

/*destroyPageFile function takes the fileName as argument and deletes the file from the current directory*/

RC destroyPageFile (char *fileName)
{
	int rc,re;
	FILE *fp;
	if(fileName != NULL)
	{
		re = remove(fileName);
		if(re == 0)
		{
			printf("------------------------FILE DELETED----------------------------\n");
			rc = RC_OK;
		}
		else
		{
			printf("----------------------UNABLE TO DELETE FILE-------------------------\n");
			rc = RC_FILE_NOT_FOUND;
		}
	}
	else
	{
		printf("----------------------FILE NAME NOT FOUND-------------------------\n");
			rc = RC_FILE_NOT_FOUND;
	}		
	return rc;
}

/*readFirstBlock function takes filehandle and memPage as arguments and reads the first block of 4096 bytes of memory form the file and returns RC_OK if operation is completed*/

RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	int rc = RC_FILE_HANDLE_NOT_INIT,size;
	int i;
	FILE *fp = fopen(fHandle->fileName,"r");
	if(fHandle != NULL)
	{
		if(fp != NULL)
		{
			fseek(fp,0,SEEK_SET);			
			size = fread(memPage,1,PAGE_SIZE,fp);
			if(size == PAGE_SIZE)
			{
				printf("----------------------FILE READ--------------\n");
				rc = RC_OK;
			}
			else
			{
				rc = RC_READ_NON_EXISTING_PAGE;
			}
		}
		else
		{
			printf("----------------------FILE NAME NOT FOUND-------------------------\n");
			rc = RC_FILE_NOT_FOUND;
		}
		
	}
	return rc;
}
	
/* readBlock function takes pageNum,fhandle and character arry as arguments and writes the block of memory(memPage) in the given pagenumber to the file and returns RC_OK if operation is completed. */

RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	
	int rc=RC_FILE_HANDLE_NOT_INIT,size=0,newSize = 0,i = 0,j = 0,sizeWriten;
	char ch;
	FILE *fp;
	fp = fopen(fHandle->fileName,"r+");
	if(fHandle != NULL)
	{
		if(fp != NULL)
		{
			fseek(fp,(pageNum)*PAGE_SIZE,SEEK_SET);
			size = fwrite(memPage,1,PAGE_SIZE,fp);
			fHandle->curPagePos = fHandle->curPagePos + size;
			printf("------------------------------CURRENT POSITION IS :: %d----\n  ",fHandle->curPagePos);
			fHandle->totalNumPages = fHandle->totalNumPages + 1;
			fHandle->mgmtInfo = memPage;
			printf("-----------------------------TOTAL PAGES------------:: %d----\n",fHandle->totalNumPages);
			if(size == PAGE_SIZE)
			{
				printf("----------------------FILE WRITE COMPLETE--------------\n");
				rc = RC_OK;
			}
			else
			{
				rc = RC_WRITE_FAILED;
			}
			fclose(fp);

		}
		else
		{
			printf("----------------------FILE NAME NOT FOUND-------------------------\n");
			rc = RC_FILE_NOT_FOUND;
		}		
	}
	

	return rc;


}

/* readBlock function takes pageNum,fhandle and character arry as arguments and reads the block of memory with the given pagenumber from the file and returns RC_OK if operation is completed. */

RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	int rc = RC_FILE_HANDLE_NOT_INIT,size;
	FILE *fp = fopen(fHandle->fileName, "r");
	
	if (fHandle != NULL)
	{
		if(fp != NULL)
		{
			fseek(fp,pageNum*PAGE_SIZE,SEEK_SET);			
			size = fread(memPage,1,PAGE_SIZE,fp);
			fHandle->curPagePos = fseek(fp,pageNum*PAGE_SIZE,SEEK_SET)+PAGE_SIZE;
			if(size==PAGE_SIZE)
			{
					printf("-----------------------------READING COMPLETE---------------------------\n");
					rc = RC_OK;
			}
		}
		fclose(fp);

	}
	return rc;
}

/* readPrevBlock function takes fhandle and character arry as arguments and reads the previous block of 4096 bytes of memory page from the current postion of the file and returns RC_OK if operation is completed. */

RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    int rc = RC_NO_PRIVIOUS_BLOCK,size;
    FILE *fp = fopen(fHandle->fileName, "r");
    if (fHandle != NULL)
    {
        if(fp != NULL)
        {
        	if(fHandle->curPagePos < PAGE_SIZE){
		printf("-------------------First block, no privious block available---------------\n");		
		}
		else
		{
			rc = readBlock((fHandle->curPagePos/PAGE_SIZE)-2,fHandle,memPage);
	        }    
        }
	fclose(fp);
    }
	return rc;
}

/* readNextBlock function takes fhandle and character arry as arguments and reads the nextblock of 4096 bytes of memory page from the current postion of the file and returns RC_OK if operation is completed. */
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	int rc = RC_NO_NEXT_BLOCK,size;
	FILE *fp = fopen(fHandle->fileName, "r");
	printf("totalNumPages:: %d---\n",fHandle->totalNumPages);
	if (fHandle != NULL)
	{
		if(fp != NULL)
	        {
	        	if(fHandle->curPagePos < PAGE_SIZE)
			{
				printf("-------------------First block, no privious block available---------------\n");		
			}
			else
			{
				rc = readBlock((fHandle->curPagePos/PAGE_SIZE)+1,fHandle,memPage);
	        	}
			fclose(fp);    
        	}
	}
}

/* readLastBlock function takes fhandle and character arry as arguments and returns RC_OK if operation is completed. */

RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	int rc ,size;
	FILE *fp = fopen(fHandle->fileName, "r");
	if (fHandle != NULL)
	{
		if(fp != NULL)
		{
			fseek(fp,(fHandle->totalNumPages-1)*PAGE_SIZE,SEEK_SET);
			size = fread(memPage,1,PAGE_SIZE,fp);
		    	if(size==PAGE_SIZE)
			{
			    printf("--------------------Done last page read---------------------\n");
			    
			}
		rc = RC_OK;
		}
	}
	fclose(fp);
	return rc;
}

/* readCurrentBlock function takes fhandle and character arry as arguments and reads data from the file and returns RC_OK if operation is completed. */

RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
int rc ,size,i,start=0;
FILE *fp = fopen(fHandle->fileName, "r");
if (fHandle != NULL)
    {
        if(fp != NULL)
        {
	printf("------------------Current page possitin: %d-----------------------------\n",fHandle->curPagePos );
	printf("------------------Current page possitin/page_size number is : %d-------------------\n",(fHandle->curPagePos/PAGE_SIZE-1) );
	fseek(fp,(fHandle->curPagePos/PAGE_SIZE-1)*PAGE_SIZE,SEEK_SET);
	rc = readBlock(fHandle->curPagePos/PAGE_SIZE-1,fHandle,memPage);	
	
        }
	fclose(fp);
   }
   return rc;

}

/* getBlockPos function takes fhandle as argument and prints the currentPagePostion and returns RC_OK if operation is completed. */
RC getBlockPos (SM_FileHandle *fHandle)
{
	int rc = RC_FILE_HANDLE_NOT_INIT;
	if (fHandle !=NULL)
	{
		printf("\n------------BLOCK POSITION IS :: %d------------------ \n",fHandle->curPagePos);
		rc = RC_OK;

	}
	return rc;
}

/* appendEmptyBlock function takes fhandle as argument and appends and empty page of 4096 bytes to the existing file and returns RC_OK if operation is completed. */

RC appendEmptyBlock (SM_FileHandle *fHandle)
{
	int rc = RC_FILE_HANDLE_NOT_INIT,ch,i,size=0;
		FILE *fp = fopen(fHandle->fileName,"r+");
		ch = fseek(fp,fHandle->curPagePos,SEEK_SET);
		printf("--------------ch:: %d------\n",ch);
		if(ch==0)
		{
			for(i = 0; i < 4096;i++ )
			{
				fputc('\0',fp);
				size = size + 1;
			}
			
			if(size == PAGE_SIZE)
			{
				fHandle->totalNumPages = fHandle->totalNumPages + 1;
				rc = RC_OK;
			}
		}
		else
		{
			printf("-------------------FSEEK NOT SUCCESSFUL-------------\n");
		}
		fclose(fp);
	
	return rc;
}

/* ensureCapacity function takes numberOfPages and FileHandle as arguments and checks if the current file pages are more then the numberofPages arugument and adds those may empty pages each of 4096 bytes to the existing file and returns RC_OK if operation is completed. */


RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
{
	int rc = RC_FILE_HANDLE_NOT_INIT,ch,i,filePages,pagesToCreate,size=0;
	if(fHandle != NULL)
	{
		filePages = fHandle->totalNumPages;
		FILE *fp = fopen(fHandle->fileName,"a");
		if(numberOfPages > filePages)
		{
			pagesToCreate = numberOfPages - filePages;			
			ch = fseek(fp,0,SEEK_END);
			
			for(i = 0; i < pagesToCreate * PAGE_SIZE ;i++ )
			{
				fputc('\0',fp);
				size = size + 1;
				fHandle->curPagePos = fHandle->curPagePos + 1;
			}
			if(size == pagesToCreate * PAGE_SIZE)
			{
				fHandle->totalNumPages = fHandle->totalNumPages + pagesToCreate;
				rc = RC_OK;
			}
			
		}
		else
		{
			printf("-----------------------NUMBER OF PAGES IS NOT GREATER THAN THE FILE PAGES--------");
		}
		fclose(fp);
	}
	return rc;
}

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	static int rc = RC_FILE_HANDLE_NOT_INIT;
	if(fHandle != NULL)
	{
		printf("-----------------------Current postion is :: %d---------------", fHandle->curPagePos/PAGE_SIZE+1);
		rc = writeBlock((fHandle->curPagePos/PAGE_SIZE)-1,fHandle,memPage);
	}
	return rc;
}
