#include<stdio.h>
#include<string.h>

#include "record_mgr.h"
#include "tables.h"
#include "dberror.h"
#include "storage_mgr.c"
#include "buffer_mgr_stat.h"
#include "rm_serializer.c"
#include "buffer_mgr.c"


static int tableId = 1;
SM_FileHandle fHandle;
SM_PageHandle pHandle; //= (SM_PageHandle) malloc(4096);
Schema *s;
static int attr;
void * mgmtData_;
static int offset = 0;
int pageNum = 2;
int slot = 1;
char *filename; 
int flag = 1;
BM_PageHandle *h;

Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
{
	printf("................................Inside create schema.....................\n");
	Schema *schema = (Schema *)malloc(sizeof(Schema));
	schema->numAttr = numAttr;
	schema->attrNames = attrNames;
	schema->dataTypes = dataTypes;
	schema->typeLength = typeLength;
	schema->keySize = keySize;
	schema->keyAttrs = keys;
	s = schema;
	return schema;
		
}
RC initRecordManager (void *mgmtData)
{
	int RC = RC_OK;
	printf("...............................Inside Init Record manager..................\n");
		
	mgmtData_ = mgmtData;
	return RC;
}
RC shutdownRecordManager ()
{
	int RC = RC_OK;
	printf("...............................Inside Shutdown Record manager..................\n");
		
	
	return RC;
}
RC createTable (char *name, Schema *schema)
{
	int RC = RC_OK;
	int rc;
	printf("...............................Inside create Table method..................\n");

	//createPageFile(name);

	//create a table file with given name
	//Check condition if errors occors 
	pHandle = (SM_PageHandle) malloc(PAGE_SIZE);
	//printf("after malloc;");
	pHandle = serializeSchema(schema);
	//printf("after schema");
	if( createPageFile(name) == 0)
	{
		//printf("createPageFile\n");
		if(openPageFile(name,&fHandle) ==0)
		{
			writeBlock (0, &fHandle, pHandle);	
		}
		closePageFile (&fHandle);
	}
	free(pHandle);
	return RC_OK;
}


int getRecordSize(Schema *schema) {

    int recordSizeBytes = 0;
    int i;
    for (i=0; i < schema->numAttr; i++) {
        switch (schema->dataTypes[i]) {
        case DT_INT:
            recordSizeBytes = recordSizeBytes + sizeof(int);
            break;
        case DT_STRING:
            recordSizeBytes = recordSizeBytes + schema->typeLength[i];
            break;
        case DT_FLOAT:
            recordSizeBytes = recordSizeBytes + sizeof(float);
            break;
        case DT_BOOL:
            recordSizeBytes = recordSizeBytes + sizeof(bool);
            break;
        }
    }
    return recordSizeBytes;
}
RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond)
{	
		int RC = RC_OK;
		printf("...........................................inside StartScan........................\n");
		return RC;
}
RC next (RM_ScanHandle *scan, Record *record)
{	
		int RC = RC_OK;
		printf("...........................................inside next........................\n");
		return RC;
		
}

RC closeScan (RM_ScanHandle *scan)
{
	int RC = RC_OK;
	printf("...........................................inside closeScan........................\n");
	return RC;
}
RC openTable (RM_TableData *rel, char *name)
{
	int RC = RC_OK;
	printf("..................................................open Table...............................\n");	
	BM_BufferPool *bm = (BM_BufferPool *) malloc (sizeof(BM_BufferPool));
	h = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));
	h->data = calloc(1,4096);
		
	if(bm != NULL)
	{
		printf("inside bm");
		filename = name;
    		initBufferPool(bm, name, 3, RS_FIFO, NULL);
		rel->schema = s;
		rel->name = name;		
		rel->mgmtData = bm;
		//appendEmptyBlock (&fHandle);
		free(bm);
	
	}
	return RC;
}
RC createRecord (Record **record, Schema *schema)
{
	int RC = RC_OK;
	attr =1;
	//*record =  malloc (sizeof(RID) + getRecordSize(schema) );
	*record =  malloc (sizeof(RID) + sizeof(char *));
	printf("size of record. :: %d\n",getRecordSize(schema));
	(*record)->data = malloc (sizeof(getRecordSize(schema)));
	printf("...........................................inside createRecord........................\n");
	//record = &r;
	//&record->id.slot = 1;
	//printf("slot :: %d\n",&record->id.slot);
	
	
	return RC;
	
}
RC setAttr (Record *record, Schema *schema, int attrNum, Value *value)
{
	int i,j,RC = RC_OK,start,end;
	char result[4];
	char * r, *r1;
	int offset = 0;
	printf("...........................................inside setAttr........................\n");
	attrOffset(schema, attrNum, &offset);
	//printf("offset :: %d",offset);
	switch(attrNum)
	{
		case 0:
		{
			sprintf(result,"%d",value->v.intV);
			printf("result :: %s\n",result);
			r = result;
			memcpy(record->data + offset, r  , sizeof(int));
			break;
		}
		case 1:
		{
			sprintf(result,"%s",value->v.stringV);
			//printf("string value:: %s\n",value->v.stringV);
			//printf("result :: %s\n",result);
			//printf("schema->typeLength[1] :: %d\n", schema->typeLength[1]);	
			r = result;
			memcpy(record->data + offset , r , schema->typeLength[1]);
			break;
		}
		case 2:
		{
			//record->data = result;
			sprintf(result,"%d",value->v.intV);
			//printf("result :: %s\n",result);
			r = result;
			memcpy(record->data + offset , r , sizeof(int));
			break;			
		}
	}
	//printf("record->data :: %s\n",record->data);	
	return RC;
}
void freeVal (Value * val)
{
	//free(val->v);
	free(val);
}
RC insertRecord (RM_TableData *rel, Record *record)
{
	int i,RC = RC_OK;
	char result[12];
	char *r;
	//BM_BufferPool *bm = (BM_BufferPool *) malloc (sizeof(BM_BufferPool));
	printf("name is :%s",filename);
	printf("************************************************************** ::: %d",pageNum);
	/*if(flag == 1)
	{
		initBufferPool(bm, filename, 3, RS_FIFO, NULL);
				
		flag = 2;
	}*/	
	//pinPage(rel->mgmtData,h,pageNum);
	
	record->id.page = pageNum;
	record->id.slot = slot;
	printf("after record id\n");
	//printf("h->pageNum :: %d\n",h->pageNum);
	sprintf(result,"%s",record->data);
	r = result;	
	printf("h->data :: ");
	printf("offset :: %d",offset);	
	memcpy(h->data + offset , r ,getRecordSize(rel->schema));
	for(i=0;i<12;i++)
	{
		//h->data [i+offset] = record->data[i]; 
		//x[i+offset] = h->data[i];
		printf("%c",h->data[i]);
	}
	offset = offset + 16;
	slot = slot + 1;
	if(slot == 256)
	{
		printf("\ninside if slot\n");
		pageNum = pageNum + 1;
	}
	markDirty(rel->mgmtData,h);	
	unpinPage(rel->mgmtData, h);
	free(h);	
	return RC_OK;
}
