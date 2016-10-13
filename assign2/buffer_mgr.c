#include "buffer_mgr.h"
#include "dberror.h"

#define RC_BM_PAGE_NOT_FOUND 100
#define RC_BM_PAGE_FOUND 101
#define RC_BM_PAGE_INSERTED 102
#define RC_BM_PAGE_NOT_INSERTED 103


/*custom defined structure to store bm->numberofPages..this is used as buffer manager*/
typedef struct UD_Bufferpool
{
	const BM_BufferPool *bm;
	bool dirty[10];	
	BM_PageHandle bp[10];
	int inserted[10];	
	int fixcount[10];
	int lruConstant[10];
	
}UD_Bufferpool;

SM_FileHandle fHandle;
SM_PageHandle sHandle;
UD_Bufferpool *udbp;
static int timeCounter=0;
static int maxNum = -1;
static int readCount =0;
static int writeCount =1;
	
char str[10];
/*to initialise bufferpool, pagecontent and custom defined bufferpool*/
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData)
{
	int i;
	printf("...............INSIDE INIT........................\n");
	bm->pageFile=pageFileName;
	bm->numPages = numPages;
	bm->strategy = strategy;
	//printf("Strategy ----- %d\n",strategy);
	bm->mgmtData = stratData;
	openPageFile (bm->pageFile, &fHandle);
	udbp = (UD_Bufferpool *) malloc(sizeof(BM_BufferPool)+(sizeof(BM_PageHandle)*numPages)+(sizeof(int)*numPages)+(sizeof(bool)*numPages)+(sizeof(int)*numPages));
	//printf("after Malloc");
	udbp->bm = bm;
	for(i = 0; i<numPages; i++)
	{
		udbp->bp[i].pageNum = -1;
		//printf("udbp->bp...........%d\n",(int)sizeof(udbp->bp[i]));
		udbp->bp[i].data = NULL;
		udbp->inserted[i]=0;
		udbp->dirty[i] = false;	
		udbp->fixcount[i] =0;
		udbp->lruConstant[i]=0;
	}
	

	return RC_OK;
}
/*to shutdown bufferpool, pagecontent and custom defined bufferpool*/
RC shutdownBufferPool(BM_BufferPool *const bm)
{
	int i;
	printf("....................INSIDE SHUTDOWN BUFFER POOL......\n");
	bm->pageFile = "";
	bm->numPages = 0;
	bm->strategy = 5;
	bm->mgmtData = NULL;
	udbp->bm = NULL;
	for(i = 0; i< bm->numPages; i++)
	{
		udbp->dirty[i] = false;
		udbp->bp[i].pageNum = -1;
		udbp->inserted[i] = 0;
		udbp->fixcount[i] = 0;
		udbp->lruConstant[i] = 0;
	}
	//free(udbp);
	maxNum = -1;
	readCount = 0;
	writeCount = 0;
	return RC_OK;
}


RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page,const PageNumber pageNum)
{
	int i,min=0,max,minVal=0,fix=0;
	int RC = RC_BM_PAGE_NOT_FOUND;
	sHandle = (SM_PageHandle) malloc(PAGE_SIZE);
	int lruPos=0,minLru,flag=0;
	printf("....................INSIDE PIN PAGE FUNCTION......\n");
	for(i=0;i<bm->numPages;i++)
	{
		//printf("bm->numPages-----%d\n",bm->numPages);
		if(udbp->bp[i].pageNum == pageNum)
		{
			page->data = udbp->bp[i].data;
			udbp->fixcount[i] = udbp->fixcount[i] + 1;
			timeCounter = timeCounter + 1;
			udbp->lruConstant[i] = timeCounter;
			RC = RC_BM_PAGE_FOUND;
			printf("udbp->fixcount[i]............::%d\n",udbp->fixcount[i]);
			
		}
	}
	//readFirstBlock (&fHandle,sHandle);
	//readBlock (&fHandle,sHandle);
	if(ensureCapacity (pageNum, &fHandle) == RC_OK)
	{
		readBlock (pageNum, &fHandle, sHandle);
	}
	page->pageNum = pageNum;
	sprintf(str, "%s-%i", "Page",pageNum);				
	page->data = str;
	if(RC == RC_BM_PAGE_NOT_FOUND)
	{
		readCount = readCount +1;
		for(i = 0;i < bm->numPages;i++)
		{
			printf("i is :: %d\n",i);
			printf("udbp->bp[i].pageNum......%d\n",udbp->bp[i].pageNum);
			if(udbp->bp[i].pageNum == -1 && udbp->bp[i].data == NULL && RC != RC_BM_PAGE_INSERTED )
			{

				printf("**************************inside if block-- space available\n");
				udbp->bp[i].pageNum = pageNum;
				sprintf(str, "%s-%i", "Page",pageNum);
				udbp->bp[i].data = str;		
				maxNum = maxNum + 1;
				udbp->inserted[i] = maxNum;
				udbp->fixcount[i] = udbp->fixcount[i] + 1; 
				timeCounter = timeCounter + 1;
				udbp->lruConstant[i] = timeCounter;
				RC = RC_BM_PAGE_INSERTED;
				printf("udbp->fixcount[i]............::%d\n",udbp->fixcount[i]);
				//printf("lruConstant[i]***************-------%d\n",timeCounter);
			}

			else if( maxNum >= bm->numPages-1 && RC != RC_BM_PAGE_INSERTED )
			{
				if(bm->strategy == RS_FIFO)
				{
					printf("**************************inside else if block FIFO method \n");
						
					minVal = udbp->inserted[0];	
						
					fix = udbp->fixcount[0];	
					for(i=1;i<bm->numPages;i++)
					{
						
						if(udbp->inserted[i]< minVal && udbp->fixcount[i]==0)
						{
							minVal = udbp->inserted[i];
							min = i;
						}
					}		
					
						
					udbp->bp[min].pageNum = pageNum;
					sprintf(str, "%s-%i", "Page",pageNum);
					udbp->bp[min].data = str;		
					maxNum = maxNum + 1;
					udbp->inserted[min] = maxNum; 
					udbp->fixcount[min] = udbp->fixcount[min] + 1;
					printf("udbp->fixcount[i]............::%d\n",udbp->fixcount[min]);
			
					RC = RC_BM_PAGE_INSERTED;
				}
				else if(bm->strategy == RS_LRU)
				{
					
					printf("------------INSIDE pin LRU");
					minLru = udbp->lruConstant[0];		
					for(i=1;i<bm->numPages;i++)
					{
						if(udbp->lruConstant[i]<minLru)
						{
							minLru = udbp->lruConstant[i];
						}	
						
					}
					//printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^minLRU :: %d",minLru);
					for(i=0;i<bm->numPages;i++)
					{
						if(udbp->lruConstant[i] == minLru && flag == 0)
						{
							lruPos = i;
							flag =1;
						}
					}
					udbp->bp[lruPos].pageNum = pageNum;
					//printf("udbp->bp[min]**********::: %d\n",udbp->bp[lruPos].pageNum);
					//printf("udbp->fixcount**********::: %d\n",udbp->fixcount[lruPos]);
					//udbp->bp[min].data = "Page-"+pageNum;//page->data;
					sprintf(str, "%s-%i", "Page",pageNum);
					udbp->bp[lruPos].data = str;		
					printf("udbp->bp[i].data............::%s\n",udbp->bp[lruPos].data);
					maxNum = maxNum + 1;
					udbp->inserted[lruPos] = maxNum; 
					udbp->fixcount[lruPos] = udbp->fixcount[lruPos] + 1;
					printf("udbp->inseted[i]..............%d::%d\n",lruPos,udbp->inserted[lruPos]);
					timeCounter = timeCounter + 1;
					udbp->lruConstant[lruPos] = timeCounter;
					//printf("max...........%d\n",maxNum);
					RC = RC_BM_PAGE_INSERTED;			
					
				}	
			}
			//continue;
		   //printf("RC........%d\n",RC);
		}
		free(sHandle);
		
	}
	return RC_OK;
}
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
{
	int i;
	printf("....................INSIDE MARK DIRTY FUNCTION......\n");
	for(i =0 ; i< bm->numPages; i++)
	{
		if(udbp->bp[i].pageNum == page->pageNum)
		{
			udbp->dirty[i] = true;
			//printf("i:: %d\n",i);
			//printf("udbp->dirty[i]:: %d\n",udbp->dirty[i]);
			
		}
	}
	return RC_OK;
}
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
	int i,temp;
	printf("....................INSIDE unpin FUNCTION......\n");
	for(i =0 ; i< bm->numPages; i++)
	{
		if(udbp->bp[i].pageNum == page->pageNum)
		{
			udbp->fixcount[i] = udbp->fixcount[i] - 1;
			//printf("i:: %d\n",i);
			//Write content to disk;
			if(udbp->dirty[i] == true && udbp->fixcount[i]==0)
			{
				//scanf("%d",&temp);
				writeCount = writeCount + 1;
				udbp->dirty[i] == false;	
			}	
			//printf("udbp->fixcount[i]............::%d\n",udbp->fixcount[i]);
			
			
		}
	}	
	return RC_OK;
}


int getNumWriteIO (BM_BufferPool *const bm)
{
	
	printf("--------------INSIDE 0GETNUMREADIO----------------\n");
	printf("writeCount::%d",writeCount);
	return writeCount;
}
int getNumReadIO (BM_BufferPool *const bm)
{
	printf("--------------INSIDE GETNUMWIRTEIO----------------\n");
	printf("writeCount::%d",readCount);
	return readCount;
}
bool *getDirtyFlags (BM_BufferPool *const bm)
{
	int i ;
	bool *dirt;
	bool d[10];
	printf("--------------INSIDE GETDIRTYFLAGS----------------\n");
	for(i = 0 ; i< bm->numPages; i++)
	{
		d[i] = udbp->dirty[i];
		//printf("d[i]::%d\n",d[i]);
	}
	dirt = d;
	return dirt;
}
int *getFixCounts (BM_BufferPool *const bm)
{
	int i;
	int *fixCounts;
	int f[10];
	//printf("------------bm->pageNum------------::%d",bm->numPages);
	printf("--------------INSIDE GETFIXCOUNTS----------------\n");
	
	for(i =0; i< bm->numPages; i++)
	{
		f[i] = udbp->fixcount[i];
		//printf("f[i]::::: %d\n",udbp->fixcount[i]);
	}
	fixCounts = f;
	
	return fixCounts;
	
}
/*returns arry of pagenumbers of pageframes*/ 
PageNumber *getFrameContents(BM_BufferPool *const bm)
{
	int i;
	PageNumber *page; 
	int p[10];
	printf("------------INSIDE GETFRAME CONTENTS FUNCTION.................\n");
	for(i=0;i<bm->numPages;i++)
	{
		p[i] = udbp->bp[i].pageNum;
		//printf("p[i]:: %d\n",p[i]);
	}
	page = p;
	//printf("-------------page[1]------:: %d\n",page[1]);
	return page;

}
RC forceFlushPool(BM_BufferPool *const bm)
{
	int i;
	printf("-----------------------INSIDE FORCE FLUSH POOL METHOD........\n");
	for(i=0;i<bm->numPages;i++)
	{
			//no need to wirte to disk;
			if(udbp->dirty[i] == true)
			{
				//writeCount = writeCount + 1;
				//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!wirteCount:::::%d\n",writeCount);
				udbp->dirty[i]= false;
			}
			
		
		
	}
	return RC_OK;
}

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
	int i;
	printf("-----------------------INSIDE FORCE PAGE METHOD........\n");
	for(i=0;i<bm->numPages;i++)
	{
		if(page->pageNum == udbp->bp[i].pageNum && udbp->dirty[i] == true)
		{
			//write page to disk
			//writeCount = writeCount + 1;
			//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!wirteCount:::::%d\n",writeCount);
		}
	}
	return RC_OK;
}

