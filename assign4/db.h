
#include "dberror.h"
#include "record_mgr.h"


#define DBName "database"

typedef struct BlockDirectory {
    int blocknum;
    int freespace;
    int numberofrecords;
    
} BlockDirectory;

typedef struct BlockHeader {
    int blocknum;
    char *schema;
    int curentoffset;
    
} BlockHeader;

typedef struct ScanData {
    Expr *cond;
    int currRecord;
    int numRecords;
    int no_blocks;
    int currentblock;
    int parsedRecords;
} ScanData;

typedef struct Scankey {
    struct Btree *currentNode;
    int recnumber;
} Scankey;

typedef struct DB_Data
{
  char *name;
  void *mgmtData;
  struct BlockDirectory *BlockDirectory;
  struct BlockHeader *BlockHeader;
  
} DB_Data;

typedef struct Btree {
    int *keys;
    struct Btree *parent;
    struct Btree **pointers;
    RID *records;
    bool is_leaf;
    int num_keys;
    int blkNum;
    struct Btree *next; // Used for queue.
    struct Btree *prev;
} Btree;

typedef struct Btree_stat {
    void *mgmtData;
    void *fileInfo;
    int num_nodes;
    int num_inserts;
    int order;
} Btree_stat;

#define MAKE_DBData()					\
  ((DB_Data *) malloc (sizeof(DB_Data)))

#define MAKE_BlockDirectory()					\
  ((BlockDirectory *) malloc (sizeof(BlockDirectory)))

#define MAKE_BlockHeader()					\
  ((BlockHeader *) malloc (sizeof(BlockHeader)))

#define MAKE_BTREE()					\
  ((Btree *) malloc (sizeof(Btree)))

#define MAKE_BTREE_STAT()					\
  ((Btree_stat *) malloc (sizeof(Btree_stat)))

extern RC createDB();
extern RC insertTable();
extern RC deleteTable();
extern RC updateTable();
extern RC deleteDB();
