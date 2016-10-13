
/*
*   Assignment 3 : Record Manager
*	Saibabu chandramouli    (A20345775)
*	Tirth Patel             (A20320187)
*	Krutharth soni	 		(A20340570)
*	Monisha		    	    (A20320532)
*	
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include "record_mgr.h"
#include "tables.h"
#include "dberror.h"
#include "storage_mgr.c"
#include "buffer_mgr_stat.h"
#include "rm_serializer.c"
#include "buffer_mgr.c"
#include "record_mgr.h"


static int tableId = 1;
SM_FileHandle fHandle;
SM_PageHandle pHandle; //= (SM_PageHandle) malloc(4096);
Schema *s;
static int attr;
void * mgmtData_;
static int offset = 0;
int pageNum = 2,slot = 1,flag = 1;
char *filename; 
int Sslot=1,Spage=2,totalTuples = 10,tuplesScanned=0;
BM_PageHandle *h;
RM_TableData *td;


typedef struct buffer
{
	BM_PageHandle *bp;
	BM_BufferPool *bm;
	
}buffer;

/* This method creates a structure "schema" based on 
the attributes passed and returns created schema  */

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
//This method initiates the record manager 

RC initRecordManager (void *mgmtData)
{
	int RC = RC_OK;
	printf("...............................Inside Init Record manager..................\n");
	mgmtData_ = mgmtData;
	return RC;
}
/*This methods creates a table file on disk 
* with the passed name to the method.  
* We store the details about this schema on  
* the first page of the created table file.
* This details include the schema information.
*/

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

/*
* This method returns the size of the record for the specific schema. 
* Input argument is "schema" for this method, 
* depending on the data types defined for each attributes of the schema 
* this method calculates record size and returns record size in bytes.
*/

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
/*
* OpenTable is a method which opens table file by initiating 
* a buffer pool to handle content of actual table file. 
*
*/

RC openTable (RM_TableData *rel, char *name)
{
	int RC = RC_OK;
	//allocating memory to a bufferpool which will handle table data
	BM_BufferPool *bm = (BM_BufferPool *) malloc (sizeof(BM_BufferPool));
	//allocating memory to a page handle which will read page by page content
	h = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));
	h->data = malloc(4096);
		
	if(bm != NULL)
	{
		filename = name;
    		initBufferPool(bm, name, 3, RS_FIFO, NULL);
		rel->schema = s;
		rel->name = name;		
		rel->mgmtData = bm;
		rel->data = h->data;
		td = rel;
		free(bm);
	
	}
	return RC;
}
/*
* This method closes the opened table, which means 
* this will deallocate the bufferpool and page handler
*
*/

RC closeTable (RM_TableData *rel)
{	
	int RC = RC_OK;
	printf("...........................................inside close table........................\n");
	return RC;
		
}
/*
* This method will delete the table file from the disk
*/

RC deleteTable (char *name)
{
	int RC = RC_OK;
	//pageNum = 2;
	slot = 1;
	
	if(destroyPageFile (name)==0)	
	{
		printf("file deleted.........\n");
	}	
	else
	{
		
		printf("File not deleted....\n");
	}
	return RC;
}
/*
* This method simply shut down the Record manager.
*/

RC shutdownRecordManager()
{
	int RC ;
	printf("...............Inside shutdown record manager............\n");
	//free(s);
	free(td->schema);
	
	RC = RC_OK;
	return RC;
	
}
/*
* This method creates a proper structure for record for insertion/update/delete related process.
* Record is created based on the information of schema provided in the input attribute. 
*/

RC createRecord (Record **record, Schema *schema)
{
	printf("...........................................inside createRecord........................\n");
	
	int RC = RC_OK;
	attr =1;
	*record = (Record *) malloc (sizeof(Record) );
	(*record)->data = malloc(12);
	return RC;
	
}
/*
* Based on the data-types of each attributes from schema information, 
* we need to set the data field of the record. This method does this job.
* It sets attributes as pet the data types.
*/

RC setAttr (Record *record, Schema *schema, int attrNum, Value *value)
{
	int i,j,RC = RC_OK,start,end;
	char result[4];
	char * r, *r1;
	int offset = 0;
	//attrOffset is a method gives us the offset of given attrNum in  given schema.
	attrOffset(schema, attrNum, &offset);
	switch(attrNum)
	{
		case 0:
		{
			sprintf(result,"%d",value->v.intV);
			r = result;
			memcpy(record->data + offset, r  ,sizeof(int));
			break;
		}
		case 1://DT_STRING:
		{
			
			memcpy(record->data + offset ,value->v.stringV, 4);
			
			break;
		}
		case 2://DT_FLOAT:
		{
			sprintf(result,"%d",value->v.intV);
			r = result;
			memcpy(record->data + offset, r  ,sizeof(int));
			break;
						
		}
		case 3://3
		{
			printf("Case - 3\n");
			sprintf(result,"%d",value->v.boolV);
			memcpy(record->data + offset , r ,sizeof(int));
			break;			
		}
	}
	return RC;
}

/*
* This method gets the value of attribute from the given record and attributeNum
* according to the schema structure
*/

RC getAttr (Record *record, Schema *schema, int attrNum, Value **value)
{
	int i,j,RC = RC_OK,start,end;
	char result[4];
	int offset = 0;
	int x1[4];
	attrOffset(schema, attrNum, &offset);
	(*value) = (Value *) malloc(sizeof(Value));
	switch(attrNum) //schema->dataTypes[attrNum]
	{
		case 0:
		{
			memcpy(result,record->data+offset,4);
			(*value)->dt = DT_INT;
			(*value)->v.intV = atoi(result);			
			printf("%d\n",(*value)->v.intV);
			break;
		}
		case 1:
		{
			memcpy(result, record->data + offset  ,4);
			(*value)->dt = DT_STRING;
			(*value)->v.stringV = result;
			printf("%s\n",(*value)->v.stringV);
			break;
		}
		case 2://2
		{
			memcpy(result,record->data+offset,4);
			(*value)->dt = DT_INT;
			(*value)->v.intV = atoi(result);			
			printf("%d\n",(*value)->v.intV);
			break;
		}
		case 3://3
		{
			memcpy(result,record->data+offset,4);
			(*value)->dt = DT_BOOL;
			(*value)->v.boolV = atoi(result);			
			break;			
		}
	}
	return RC;
}

/*
* Insert Record method insert the created method into the "rel", 
* which has buffer pool, so we insert records into this pool and 
* then we make this page as dirty, and finally we unpin the page
* to write it on disk.
*/ 

RC insertRecord (RM_TableData *rel, Record *record)
{
	int i,RC = RC_OK;
	char result[25];
	char *r,*r1;
	char l1[4],l2[4];	
	pinPage(rel->mgmtData,h,pageNum);
	record->id.page = pageNum;
	record->id.slot = slot;
	sprintf(&result[0],"[");
	sprintf(l1,"%d", pageNum);
	memcpy(result+1,l1,sizeof(int));
	sprintf(&result[5],"-");
	sprintf(l2,"%d", slot);
	r = l2;
	memcpy(result+6,l2,sizeof(int));
	sprintf(&result[10],"]");
	sprintf(&result[11],":");
	
	memcpy(result+12,record->data,12);	
	r = result;
	memcpy(rel->data + offset , result ,24);
	offset = offset + 24;
	slot = slot + 1;
	h->data = rel->data;
	markDirty(rel->mgmtData,h);	
	unpinPage(rel->mgmtData, h);
	if(slot*24 >= 4096)
	{
		pageNum = pageNum + 1;
		offset =0;
		slot = 1;
		free(h->data);
		h = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));
		h->data = malloc(4096);
		rel->data = h->data;
	}	
	return RC_OK;
}
/*
* This method gives us the record based on the RID from the table data.
*/

RC getRecord (RM_TableData *rel, RID id, Record *record)
{
	int RC = RC_OK,i,j=12,k,x,x1,s=0;
	int pageL,slotL,len;
	char result[12];
	char s1[4];
	char c1[4],c2[1];
	
	printf("\n");
	printf("...........................................inside getRecord........................\n");
	pinPage(rel->mgmtData,h,id.page);
	if(openPageFile(rel->name,&fHandle) ==0)
	{
		readBlock (id.page, &fHandle, pHandle);	
	}
	memcpy(s1,pHandle+1,sizeof(int));
	x = atoi(s1);
	memcpy(&slotL,pHandle+6,4);
	
	s = (id.slot-1)*24 + 12;
	if(x == id.page)
	{
		memcpy(record->data, pHandle+s,12);
	}
	unpinPage(rel->mgmtData,h);
	return 0;
	
}
/*
* Free the memory allocated for the record
*/

RC freeRecord (Record *record)
{
	int RC = RC_OK;
	printf("----------------------------------------inside free Record............................\n");
	record->id.slot =0 ;
	record->id.page = 2 ;
	free(record->data);
			
	return RC;
}
/*
* Updates the content of the record based on the targeted RID.
*/

RC updateRecord (RM_TableData *rel, Record *record)
{
	int RC = RC_OK,s=0;
	printf("----------------------------------inside update record method......................\n");
	Record *result = malloc(sizeof(Record));
	RID *rid = (RID *) malloc(sizeof(RID));
  	rid->page = record->id.page;
	rid->slot = record->id.slot;
	
	pinPage(rel->mgmtData,h,record->id.page);
	if(openPageFile(rel->name,&fHandle) ==0)
	{
		readBlock (record->id.page, &fHandle, pHandle);	
	}	
	s = (record->id.slot-1)*24 + 12;
	memcpy(pHandle+s,record->data,12);
	h->data = pHandle;
	markDirty(rel->mgmtData,h);	
	unpinPage(rel->mgmtData, h);
	
	return RC;	
}
/*
* Delete the specific record in table file based on the RID. 
*/

RC deleteRecord (RM_TableData *rel, RID id)
{
	int i,RC = RC_OK,s=0;
	char *empty = calloc(1,24);
	printf("\n----------------------------------inside delete record method......................\n");
		
	
	pinPage(rel->mgmtData,h,id.page);
	//getchar();
	if(openPageFile(rel->name,&fHandle) ==0)
	{
		readBlock (id.page, &fHandle, pHandle);	
	}
	s = (id.slot-1)*24;
	memcpy(pHandle+s,empty,24);
	h->data = pHandle;
	markDirty(rel->mgmtData,h);	
	unpinPage(rel->mgmtData, h);
	return RC;
	
}
/*
* This method starts the scaning process
*/

RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond)
{
	int RC = RC_OK,i;
	printf(".................................Insde Start scan method.....................................\n");
	scan->rel = (RM_TableData *)malloc(sizeof(RM_TableData));
	
	scan->rel = rel;
	scan->mgmtData = cond;	
	
	Sslot = 1;
	Spage = 2;
	tuplesScanned = 0;
		

	return RC;	
}
/*
* This method seek for the next record for the scan
*
*/

RC next (RM_ScanHandle *scan, Record *record)
{	
		int RC = 9;
		printf("...........................................inside next........................\n");
		Value **value;
		value=malloc(sizeof(**value));
		(*value)=malloc(sizeof(value));

		Record *rt = (Record *)malloc(sizeof(Record));
		rt->data = malloc(12);
		RID *rid = (RID *)malloc(sizeof(RID));				
		
		rt->id.page = Spage;
		rt->id.slot = Sslot;
		rid->page = Spage;
		rid->slot = Sslot;
		
		(*value)->dt=DT_BOOL;
		(*value)->v.boolV=-1;
			
		
		while(tuplesScanned <= totalTuples && RC != RC_OK)
		{
			//getchar();
			rt->id.slot = Sslot;
			rid->slot = Sslot;
			scan->num = tuplesScanned;
			if(getRecord (scan->rel, *rid,rt) == 0)
			{
				evalExpr (rt, scan->rel->schema, scan->mgmtData, value);
				if((*value)->v.boolV)
				{
					memcpy(record->data,rt->data,12);
					RC = RC_OK;
					
				}
				
			}
			tuplesScanned = tuplesScanned + 1;
			Sslot = Sslot + 1;
			
		}
		
		if(tuplesScanned > totalTuples)
		{
			return RC_RM_NO_MORE_TUPLES;
		}						
		//
		free(*value);
		return RC;
		
}
/*
* This method closes the scan process
*
*/

RC closeScan (RM_ScanHandle *scan)
{
	int RC = RC_OK;
	printf("...........................................inside closeScan........................\n");
	return RC;
}


