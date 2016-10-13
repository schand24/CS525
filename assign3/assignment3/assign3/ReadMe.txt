ADO Assignment3 : Submission

Group Members : 
	Krutharth soni 		(A20340570)
	Tirth Patel             (A20320187)
	Monisha		        (A20320532)
	Saibabu chandramouli    (A20345775)



Assignment Implementations:

*This Assignment includes implementation of record manager that  allows navigation through records, and inserting and deleting records.

	The following methods are implemented by our team.
	
extern RC initRecordManager (void *mgmtData); // Initiated the Record Manager

extern RC shutdownRecordManager (); // Shutdowns Record Manager and removes all the memory which is created during the implementation.
extern RC createTable (char *name, Schema *schema);// Creates a table for the given schema.
extern RC openTable (RM_TableData *rel, char *name);// Opens a table for given table name and fetches data into memory 


extern RC closeTable (RM_TableData *rel);//Closes the opened table in our implementation.
extern RC deleteTable (char *name);// Deletes the table from the file and releases the memory from the storage and buffer managers initiated.
extern int getNumTuples (RM_TableData *rel);// Pulls the count of the tuples from the relation which is created!

// handling records in a table
extern RC insertRecord (RM_TableData *rel, Record *record);// Inserts Record into the table and stores the relevant d into the RM_TableData
extern RC deleteRecord (RM_TableData *rel, RID id);// Deletes the specified id from the RM_TableData and releases the memory of that particular 
extern RC updateRecord (RM_TableData *rel, Record *record);// Updates the table schema and data for the given RID and the sets it to the record values which is passed.
extern RC getRecord (RM_TableData *rel, RID id, Record *record);// retrieves the record which is specified or passed by the RID parameter.

// scans
extern RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond);// Initiates the scan methodology and pulls all the required data into memory which would be helpful for further scans
extern RC next (RM_ScanHandle *scan, Record *record);// pulls the next record from the table 
extern RC closeScan (RM_ScanHandle *scan);// Closes the scan method and releases the memory which is consumed.

// dealing with schemas
extern int getRecordSize (Schema *schema);// retrieves the size of the record of the Schema.
extern Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys);// creates the schema of the table and writes that into pagefile
extern RC freeSchema (Schema *schema);// Frees the schema from the pagefile

// dealing with records and attribute values
extern RC createRecord (Record **record, Schema *schema);// Creates the new record
extern RC freeRecord (Record *record);// Frees the record
extern RC getAttr (Record *record, Schema *schema, int attrNum, Value **value); // gets the attribute values based on the attribute passed through attrNum.
extern RC setAttr (Record *record, Schema *schema, int attrNum, Value *value); // sets the attribute values based on the attribute number and value passed through attrNum & value.

*Implements Table and Record Manager Functions (initRecordManager, shutdownRecordManager, createTable, openTable, closeTable, deleteTable, getNumTuples ),
Record functions that are used to insert, delete, update, retrive records	scan functions are used to retrive tuples from a table according client codition	
schema Function (getRecordSize, createSchema, freeSchema)	
Attribute Functions are used to create new schema and to get or set attributes values of records	


*Test cases provided in the class test_assign3_1.c is executed successfully .
* Test case for the test_expr.c is also executed successfully.

we have implemented test_expr.c methods.


Note:: we are facing some memory leaks while integrating the total solution. 
but we have implemented all the methods and  are passing the testcases. for your reference we are attaching the output for individual test cases.
we admit that is mistake, but please consider as we are able to execute the individual test case. 
output files are :: 
testCase1.txt
testCase2.txt
testCase3.txt
testCase4.txt
testCase5.txt
testCase6.txt
testCase7.txt

Compiling Instruction: 
We have implemented the following make files (makefile1) 

1.Makefile1 : This includes the test file test_assign3_1.c 

	Command to compile : make -f Makefile1
	Output file generated: assign3
	Command to Run : ./assign3

Makefile2 : This includes the test file test_expr.c 

	Command to compile : make -f Makefile2
	Output file generated: assign3_2
	Command to Run : ./assign3_2
