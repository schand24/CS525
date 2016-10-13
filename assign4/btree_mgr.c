#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#include "storage_mgr.h"
#include "buffer_mgr.h"
#include "buffer_mgr_stat.h"
#include "dberror.h"
#include "dt.h"
#include "db.h"
#include "tables.h"
#include "record_mgr.h"
#include "btree_mgr.h"

void * head_node = NULL;

Btree * queue = NULL;


Btree* make_node(BTreeHandle* tree) {
    unsigned int blkNum=-1;
    Btree *new_node = MAKE_BTREE();
    Btree_stat *stat=tree->mgmtData;
    BM_PageHandle *bh = MAKE_PAGE_HANDLE();
    pinPage(stat->fileInfo, bh, BTREE_INFO_BLOCK);
    memcpy(&blkNum,bh->data,sizeof(int));
    blkNum=blkNum+1;
    if (new_node == NULL) {
        return NULL;
    }

    new_node->keys = malloc(stat->order * sizeof (int));
    new_node->records = malloc(stat->order * sizeof (void *));
    new_node->pointers = malloc((stat->order + 1) * sizeof (void *));
    new_node->blkNum=blkNum;
    new_node->is_leaf = true;
    new_node->num_keys = 0;
    new_node->parent = NULL;
    new_node->next = NULL;
    new_node->prev = NULL;
    update_btree(bh->data, tree->keyType, stat->order,0, 1);
    markDirty(stat->fileInfo,bh);
    unpinPage(stat->fileInfo,bh);
    forceFlushPool(stat->fileInfo);
    free(bh);
    return new_node;
}

RC initIndexManager(void* mgmtData) {

    return RC_OK;

}

/*
 * AUTHOR: Nikhil
 * METHOD: Helper function Update btree
 * INPUT: Btree and few other variables
 * OUTPUT: buffer
 */

RC update_btree(char *data, DataType keyType, int n,int noNodes, int type) {
    unsigned int offset = 0, noblks = 0, noEntries = 0, curBlk = 0, rBlk = 0;
    unsigned int key = -1;

    switch (keyType) {
        case DT_INT: key = 0;
            break;
        case DT_STRING: key = 1;
            break;
        case DT_FLOAT: key = 2;
            break;
        case DT_BOOL: key = 3;
            break;
    }

    switch (type) {
        case 0:
            //initial info to be stored in the header block for Index file
            memmove(data, &curBlk, sizeof (int));
            offset = offset + sizeof (int);
            memmove(data + offset, &noblks, sizeof (int));
            offset = offset + sizeof (int);
            memmove(data + offset, &noEntries, sizeof (int));
            offset = offset + sizeof (int);
            memmove(data + offset, &key, sizeof (int));
            offset = offset + sizeof (int);
            memmove(data + offset, &rBlk, sizeof (int));
            offset = offset + sizeof (int);
            memmove(data + offset, &n, sizeof (int));
            break;
        case 1:
            //When a new node is created
            memcpy(&curBlk, data, sizeof (int));
            curBlk = curBlk + 1;
            break;
        case 2:
            //when a new key is inserted
            offset = offset + sizeof (int);
            memmove(data + offset, &noNodes, sizeof (int));
            offset = offset + sizeof (int);
            memcpy(&noEntries, data + offset, sizeof (int));
            noEntries = noEntries + 1;
            memmove(data + offset, &noEntries, sizeof (int));
            break;
    }

    return RC_OK;
}


RC createBtree(char* idxId, DataType keyType, int n) {

    BM_BufferPool *bm = MAKE_POOL();
    BM_PageHandle *bh = MAKE_PAGE_HANDLE();

    //Btree *node = MAKE_BTREE();
    //create the page file
    createPageFile(idxId);
    initBufferPool(bm, idxId, 3, RS_FIFO, NULL);

    pinPage(bm, bh, BTREE_INFO_BLOCK);
    update_btree(bh->data, keyType, n,0, 0);
    markDirty(bm, bh);
    unpinPage(bm, bh);

    forceFlushPool(bm);

    //Shut down the buffer pool
    shutdownBufferPool(bm);
    free(bm);
    return RC_OK;

}


RC openBtree(BTreeHandle** tree, char* idxId) {
    unsigned int offset = 0, noblks = 0, noEntries = 0, curBlk = 0, rBlk = 0, key = -1, order = 0;
    DataType dt;
    BM_BufferPool *bm = MAKE_POOL();
    BM_PageHandle *bh = MAKE_PAGE_HANDLE();
    Btree *node;
    Btree_stat *btStat = MAKE_BTREE_STAT();
    (*tree) = (BTreeHandle *) malloc(sizeof (BTreeHandle));
    int i = 0;
    (*tree)->idxId = idxId;
    initBufferPool(bm, idxId, 3, RS_FIFO, NULL);

    pinPage(bm, bh, BTREE_INFO_BLOCK);
    //read tree from  the file
    //Readtree(tree, bh->data);

    offset = offset + sizeof (int);
    memcpy(&noblks, bh->data + offset, sizeof (int));
    offset = offset + sizeof (int);
    memcpy(&noEntries, bh->data + offset, sizeof (int));
    offset = offset + sizeof (int);
    memcpy(&key, bh->data + offset, sizeof (int));
    offset = offset + sizeof (int);
    memcpy(&rBlk, bh->data + offset, sizeof (int));
    offset = offset + sizeof (int);
    memcpy(&order, bh->data + offset, sizeof (int));


    unpinPage(bm, bh);


    switch (key) {
        case 0: dt = DT_INT;
            break;
        case 1: dt = DT_STRING;
            break;
        case 2: dt = DT_FLOAT;
            break;
        case 3: dt = DT_BOOL;
            break;
    }
    (*tree)->keyType=dt;
    btStat->num_nodes = noblks;
    btStat->num_inserts = noEntries;
    btStat->order = order;
    (*tree)->mgmtData = btStat;
    btStat->fileInfo=bm;
    btStat->mgmtData = make_node(*tree);
    head_node = btStat->mgmtData;
    
        free(bh);
        //free(bm);
    return RC_OK;
}


RC Readtree(BTreeHandle** tree, char *data) {
    unsigned int offset = 0;
    Btree *node;
    char *keyT;
    int order;
    keyT = (char *) malloc(6);
    memcpy(&order, data, sizeof (int));
    offset = offset + sizeof (int);
    //  memcpy(node, data+offset, sizeof (Btree));
    //  offset = offset + sizeof (Btree);
    memcpy(keyT, data + offset, 6);

    (*tree)->mgmtData = node;
    if (strstr(keyT, "INT")) {

        (*tree)->keyType = DT_INT;
    }
    if (strstr(keyT, "STRING")) {

        (*tree)->keyType = DT_STRING;
    }
    if (strstr(keyT, "FLOAT")) {

        (*tree)->keyType = DT_FLOAT;
    }
    if (strstr(keyT, "BOOL")) {

        (*tree)->keyType = DT_BOOL;
    }

}


RC start_new_tree(Btree *root, Value* key, RID rid) {

    root->is_leaf = true;
    root->keys[0] = key->v.intV;
    root->records[0] = rid;
    root->parent = NULL;
    root->num_keys++;
    return RC_OK;

}



RC find_insert_into_leaf(Btree *root, Value* key, RID rid) {

    int index = 0, i = 0;

    while (index < root->num_keys && root->keys[index] < key->v.intV) {
        index++; //find index such that it is less than order and value must be greater than other elements
    }

    for (i = root->num_keys; i > index; i--) {
        root->keys[i] = root->keys[i - 1];
        root->records[i] = root->records[i - 1];
    }

    root->keys[index] = key->v.intV;
    root->records[index] = rid;
    root->num_keys++;
    return RC_OK;

}


RC find_insert_into_parent(Btree *root, int key) {

    int index = 0, i = 0;

    while (index < root->num_keys && root->keys[index] < key) {
        index++; //find index such that it is less than order and value must be greater than other elements
    }

    for (i = root->num_keys; i > index; i--) {
        root->keys[i] = root->keys[i - 1];
        root->records[i] = root->records[i - 1];
    }

    root->keys[index] = key;
    root->records[index].page = 0;
    root->records[index].slot = 0;
    root->num_keys++;
    return RC_OK;

}

RC display(BTreeHandle* tree){


    Btree *root,*temp1,*temp2;
    Btree_stat *btstat;
    btstat=tree->mgmtData;
    root=btstat->mgmtData;
    int i=0,k=0;

    temp1=root->pointers[1];
    while(root!=NULL){

        for(i=0;i<root->num_keys;i++){
        printf(" %d",root->keys[i]);
        }
        printf("\t");
        root=root->next;

    }
    printf("\n");

      root=btstat->mgmtData;
    while(root->pointers[0]!=NULL){

       root=root->pointers[0];

    }
  


    while(root!=NULL){
        i=0;
    while(i<root->num_keys ){

        printf(" %d",root->keys[i]);
        i++;

    }
            printf("\t");

    root=root->next;
    }
      printf("\n");
    return RC_OK;
}


int split_node(int node_len) {
    if (node_len % 2 == 0) {
        return node_len / 2;
    } else {

        return node_len + 1 / 2;
    }
}

Btree* find_node_parent(Btree *old_node, int new_key) {

    Btree *node, *prev;
    node = old_node->parent;
    prev = node;
    while (node != NULL) {

        if (node->is_leaf = false) {

            if (node->keys[0] > new_key) {

                return prev;
            }


        }
        prev = node;
        node = node->next;
    }
    return prev;




}


RC insert_into_parent(BTreeHandle* tree, Btree_stat *root, Btree *old_node, Btree *new_node, int new_key) {

    Btree *parent_node;
    if (old_node->parent == NULL) {
        parent_node = make_node(tree);
        root->num_nodes++;
        parent_node->pointers[0] = old_node;
        parent_node->pointers[1] = new_node;
        root->mgmtData = parent_node;
    } else {
        parent_node = find_node_parent(old_node, new_key);
        int k = parent_node->num_keys;
        parent_node->pointers[k + 1] = new_node;
    }
    parent_node->is_leaf = false;

    old_node->parent = parent_node;
    new_node->parent = parent_node;


    if (parent_node->num_keys < root->order) {

        int k = 0;
        k = parent_node->num_keys + 1;
        parent_node->pointers[k] = new_node;
        find_insert_into_parent(parent_node, new_key);
        return RC_OK;

    }

    if (parent_node->num_keys == root->order) {

        Split_and_insert_parent(tree, root, parent_node, new_key);
        return RC_OK;

    }


}


RC Split_and_insert(BTreeHandle* tree, Btree_stat *root, Btree *old_node, Value* key, RID rid) {

    Btree *new_node, *temp1;
    Btree_stat *stat;
    int index = 0, i = 0, j = 0;
    int *temp_array_keys;
    RID *temp_array_pointers;
    int split_pos, num_new_node = 0, new_key = 0;

    stat = tree->mgmtData;

    if (old_node->next == NULL) {
        new_node = make_node(tree);
        stat->num_nodes++;
        old_node->next = new_node;
        new_node->prev = old_node;


        temp_array_keys = malloc((stat->order + 1) * sizeof (int));

        temp_array_pointers = malloc((stat->order + 1) * sizeof (void));

        while (index < old_node->num_keys && old_node->keys[index] < key->v.intV) {
            index++; //find index such that it is less than order and value must be greater than other elements
        }


        do {
            if (j == index) {
                j++;
            }
            temp_array_keys[j] = old_node->keys[i];
            temp_array_pointers[j] = old_node->records[i];
            i++;
            j++;
        } while (i < old_node->num_keys);

        temp_array_keys[index] = key->v.intV;
        temp_array_pointers[index] = rid;
        split_pos = 1 + split_node(stat->order);

        old_node->num_keys = 0;
        j = 0;
        for (i = 0; i < split_pos; i++) {

            old_node->keys[i] = temp_array_keys[i];
            old_node->records[i] = temp_array_pointers[i];
            old_node->num_keys++;
            j++;
        }

        num_new_node = stat->order + 1 - split_pos;

        for (i = 0; i < num_new_node; i++) {

            new_node->keys[i] = temp_array_keys[j];
            new_node->records[i] = temp_array_pointers[j];
            new_node->num_keys++;
            j++;

        }

        // root->mgmtData=new_node;
        new_node->parent = old_node->parent;

        new_key = new_node->keys[0];

        insert_into_parent(tree, root, old_node, new_node, new_key);

    } else {
        //if node is already present

        temp1 = old_node->next;
        int k = 0;
        k = old_node->num_keys;
        if (temp1->num_keys < stat->order && old_node->keys[k - 1] < key->v.intV) {
            new_node = temp1;
            find_insert_into_leaf(new_node, key, rid);
            return RC_OK;

        } else {
            new_node = make_node(tree);
            stat->num_nodes++;
            old_node->next = new_node;
            new_node->prev = old_node;
            new_node->next = temp1;
            temp1->prev = new_node;
        }

        temp_array_keys = malloc((stat->num_inserts + 1) * sizeof (int));

        temp_array_pointers = malloc((stat->num_inserts + 1) * sizeof (void));

        while (index < old_node->num_keys && old_node->keys[index] < key->v.intV) {
            index++; //find index such that it is less than order and value must be greater than other elements
        }


        do {
            if (j == index) {
                j++;
            }
            temp_array_keys[j] = old_node->keys[i];
            temp_array_pointers[j] = old_node->records[i];
            i++;
            j++;
        } while (i < old_node->num_keys);

        temp_array_keys[index] = key->v.intV;
        temp_array_pointers[index] = rid;
        split_pos = 1 + split_node(stat->order);

        old_node->num_keys = 0;
        j = 0;
        for (i = 0; i < split_pos; i++) {

            old_node->keys[i] = temp_array_keys[i];
            old_node->records[i] = temp_array_pointers[i];
            old_node->num_keys++;
            j++;
        }

        num_new_node = stat->order + 1 - split_pos;

        for (i = 0; i < num_new_node; i++) {

            new_node->keys[i] = temp_array_keys[j];
            new_node->records[i] = temp_array_pointers[j];
            new_node->num_keys++;
            j++;

        }

        root->mgmtData = new_node;
        new_node->parent = old_node->parent;

        new_key = new_node->keys[0];

        //insert key into parent

        insert_into_parent(tree, root, old_node, new_node, new_key);


    }

    return RC_OK;

}



RC Split_and_insert_parent(BTreeHandle *tree, Btree_stat *root, Btree *old_node, int key) {

    Btree *new_node;
    new_node = make_node(tree);
    root->num_nodes++;
    new_node->is_leaf = false;
    old_node->next = new_node;
    new_node->prev = old_node;
    int index = 0, i = 0, j = 0;
    int *temp_array_keys;
    int split_pos, num_new_node = 0, new_key = 0;
    temp_array_keys = malloc((root->order + 1) * sizeof (int));

    while (index < old_node->num_keys && old_node->keys[index] < key) {
        index++; //find index such that it is less than order and value must be greater than other elements
    }


    do {
        if (j == index) {
            j++;
        }
        temp_array_keys[j] = old_node->keys[i];
        i++;
        j++;
    } while (i < old_node->num_keys);

    temp_array_keys[index] = key;
    split_pos = 1 + split_node(root->order);

    old_node->num_keys = 0;
    j = 0;
    for (i = 0; i < split_pos - 1; i++) {

        old_node->keys[i] = temp_array_keys[i];
        old_node->num_keys++;
        j++;
    }
    j++;
    new_key = temp_array_keys[split_pos - 1];
    num_new_node = root->order + 1 - split_pos;

    for (i = 0; i < num_new_node; i++) {

        new_node->keys[i] = temp_array_keys[j];
        new_node->num_keys++;
        j++;

    }

    root->mgmtData = new_node;

    //new_key=new_node->keys[0];

    //insert key into parent

    insert_into_parent(tree, root, old_node, new_node, new_key);

}


Btree* find_node_to_insert(Btree_stat *root, Value* key) {

    Btree *node, *prev, *temp2;
    node = head_node;
    prev = node;
    while (node != NULL) {

        if (node->is_leaf = true) {

            if (node->keys[0] > key->v.intV) {

                return prev;
            }


        }
        prev = node;
        node = node->next;
    }
    return prev;


}

RC insertKey(BTreeHandle* tree, Value* key, RID rid) {

    Btree *node, *temp1;
    Btree_stat *root;
    int i;
    root = tree->mgmtData;
    node = root->mgmtData;
    if (root->num_nodes == 0) {
        root->num_nodes++;
        start_new_tree(node, key, rid);
        root->num_inserts++;
        updateInsertStat(tree,root);
        return RC_OK;
    }

    node = find_node_to_insert(root, key);

    temp1 = head_node;
    while (temp1 != NULL) {
        for (i = 0; i < temp1->num_keys; i++) {
            if (temp1->keys[i] == key->v.intV) {
                updateInsertStat(tree,root);
                return RC_OK;
            }
        }
        temp1 = temp1->next;
    }
    if (node->num_keys < root->order) {

        find_insert_into_leaf(node, key, rid);
        root->num_inserts++;
        updateInsertStat(tree,root);
        return RC_OK;
    }

    if (node->num_keys == root->order) {

        Split_and_insert(tree, root, node, key, rid);
        root->num_inserts++;
        updateInsertStat(tree,root);
       return RC_OK;
    }
    
}

RC updateInsertStat(BTreeHandle *bhandle,Btree_stat* stat)
{
    BM_PageHandle *bh = MAKE_PAGE_HANDLE();
    pinPage(stat->fileInfo, bh, BTREE_INFO_BLOCK);
update_btree(bh->data,bhandle->keyType,stat->order,stat->num_nodes,2);
markDirty(stat->fileInfo,bh);
unpinPage(stat->fileInfo,bh);
forceFlushPool(stat->fileInfo);
free(bh);
return RC_OK;
}



RC findKey(BTreeHandle *tree, Value *key, RID *result) {

    Btree *root, *temp1;
    Btree_stat *btstat;
    btstat = tree->mgmtData;
    root = btstat->mgmtData;

    int i = 0, k = 0;

    root = btstat->mgmtData;

    temp1 = root;
    while (temp1->is_leaf == false) {
        for (i = 0; i < root->num_keys; i++) {
            if (key->v.intV < root->keys[i]) {

                temp1 = root->pointers[i];
                break;

            } else {

                temp1 = root->pointers[i + 1];
            }

        }
    }

    for (i = 0; i < temp1->num_keys; i++) {

        if (temp1->keys[i] == key->v.intV) {

            *result = temp1->records[i];
            return RC_OK;
        }
    }

    return RC_IM_KEY_NOT_FOUND;

}



RC openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle) {

    Scankey *keydata = NULL;

    Btree_stat *treeStat;
    Btree *node;

    treeStat = tree->mgmtData;
    node = treeStat->mgmtData;

    while (node->pointers[0] != NULL) {

        node = node->pointers[0];

    }

    (*handle) = (BT_ScanHandle *) malloc(sizeof (BT_ScanHandle));
    if (*handle == NULL)
        return RC_NOMEM;

    keydata = (Scankey *) malloc(sizeof (Scankey));

    keydata->currentNode = node;
    keydata->recnumber = 0;

    (*handle)->tree = tree;
    (*handle)->mgmtData = (void *) keydata;


    return RC_OK;

}



RC nextEntry(BT_ScanHandle *handle, RID *result) {

    Btree *node;
    int numrec;
    Scankey *keydata = NULL;

    keydata = handle->mgmtData;
    node = keydata->currentNode;
    numrec = keydata->recnumber;

    if (node != NULL) {
        if (node->num_keys > numrec) {


            *result = node->records[numrec];

            numrec++;

        }
        if (numrec == node->num_keys) {

            node = node->next;
            numrec = 0;

        }

        keydata->currentNode = node;
        keydata->recnumber = numrec;

        if (result == NULL) {

            return RC_IM_NO_MORE_ENTRIES;

        }

        return RC_OK;
    } else {

        return RC_IM_NO_MORE_ENTRIES;

    }


}


RC getNumNodes(BTreeHandle *tree, int *result) {

    Btree *node, *temp1;
    Btree_stat *root;
    int i;
    root = tree->mgmtData;

    *result = root->num_nodes;

    return RC_OK;

}


RC getNumEntries(BTreeHandle *tree, int *result) {

    Btree *node, *temp1;
    Btree_stat *root;
    int i;
    root = tree->mgmtData;

    *result = root->num_inserts;

    return RC_OK;

}


RC getKeyType(BTreeHandle *tree, DataType *result) {

    if (tree->keyType == DT_INT) {
        *result = DT_INT;
    }
    if (tree->keyType == DT_STRING) {
        *result = DT_STRING;
    }
    if (tree->keyType == DT_FLOAT) {
        *result = DT_FLOAT;
    }
    if (tree->keyType == DT_BOOL) {
        *result = DT_BOOL;
    }
    return RC_OK;
}


//---------------------------------------------------------------------------------------------
//Delete

/*
 * AUTHOR: Nikhil
 * METHOD: heck for underflow
 * INPUT: Tree 
 * OUTPUT: RC_OK on SUCCESS; RC_OTHERS on FAIL;
 */

bool checkUnderflow(Btree_stat* stat,Btree * node){
    
     if (stat->order % 2 == 0){
        if(node->num_keys < (stat->order)/2){
            return true;
        }
    }
    else{

        if(node->num_keys < (stat->order+1)/2){
            return true;
        }
    }
    
     return false;

}


RC find_insert_after_redistribute(Btree *root,int key, RID *rid){

    int index=0,i=0;

    while(index< root->num_keys && root->keys[index] < key){
        index++;  //find index such that it is less than order and value must be greater than other elements
    }

    for (i = root->num_keys; i > index; i--) {
        root->keys[i] = root->keys[i - 1];
        root->records[i] = root->records[i - 1];
    }

    root->keys[index]=key;
    root->records[index]=*rid;
    root->num_keys++;
    return RC_OK;

}


RC update_parent_node(Btree *right_node,int key){

    right_node=right_node->parent;
    while(right_node!=NULL){
    
        if(right_node->is_leaf==false){
        
            right_node->keys[0]=key;
        
        }
        
        right_node=right_node->parent;
    
    }
    return RC_OK;


}


RC redistribute(Btree_stat *stat,Btree *left_node, Btree *right_node){

    int index=0,i=0,j=0;
    int *temp_array_keys;
    RID *temp_array_pointers;
    int key;
    RID *rid;

    temp_array_keys = malloc( (stat->order+1) * sizeof(int) );

    temp_array_pointers = malloc( (stat->order+1) * sizeof(void *) );

    index=left_node->num_keys-1;
    key=left_node->keys[index];
    *rid=left_node->records[index];

    left_node->keys[index]=NULL;
    left_node->records[index].page=0;
    left_node->records[index].slot=0;
    left_node->num_keys--;
    

  
    find_insert_after_redistribute(right_node,key,rid);
    
    update_parent_node(right_node,key);


    return RC_OK;
}


// Deletes an entry from the B+ tree.
RC delete_entry( BTreeHandle *tree, Btree *node, Value *key) {

    int i=0,j;
    Btree *new_node, *left_node;

    // Remove key from node.

 
    // Remove the key and shift other keys accordingly.

    while (node->keys[i] != key->v.intV){
        i++;
    }
    if(node->num_keys>1){
    for (j=i; j < node->num_keys-1; j++){
        node->keys[j] = node->keys[j+1];
        node->records[j]=node->records[j+1];
        node->pointers[j]=node->pointers[j+1];
 
    }

     node->pointers[j]=node->pointers[j+1];
     node->num_keys--;
    }

    else{
    
        node->keys[i] =NULL;
        node->records[i].slot=0;
        node->records[i].page=0;
        node->pointers[i]=NULL;
        node->num_keys--;
        if(node->num_keys!=0){
        merge_nodes(node->prev,node);
        }
        node=NULL;
    
    }
 

    return RC_OK;
}


Btree*   find_leaf(BTreeHandle *tree, Value *key) {
   Btree *root,*temp1;
    Btree_stat *btstat;
    btstat=tree->mgmtData;
    root=btstat->mgmtData;

    int i=0,k=0;

    root=btstat->mgmtData;

    temp1=root;
    while(temp1->is_leaf==false){
    for(i=0;i<root->num_keys;i++){
    if(key->v.intV<root->keys[i]){
        
        temp1=root->pointers[i];
        break;
    
        }
    else{
    
        temp1=root->pointers[i+1];
        }
    
   }
  }
    
    return temp1;
    
}


RC update_parent_nodes(Btree *child){

    Btree *parent, *top_node;
    
    int i=0,j;
    parent=child->parent;
    if(parent!=NULL){
    
    for(i=0;i<parent->num_keys;i++){
        
        if(parent->keys[i]==child->keys[0]){
        
            break;  
        }
        
    
    }
    
    for(j=i;j<parent->num_keys;j++){
    
        parent->keys[j]=parent->keys[j+1];
        parent->pointers[j]=parent->pointers[j+1];

    }
    
  //  parent->pointers[j-1]=parent->pointers[j];
    if(parent->num_keys==0){
        merge_nodes(parent->prev,parent);
        return RC_OK;
    }
    update_parent_nodes(parent);
    }

    return RC_OK;

}



RC merge_nodes(Btree_stat *stat,Btree *left_node,Btree *right_node){

    int numleft,numright,index,i,j=0;
    if(left_node!=NULL){
    numleft=stat->order-left_node->num_keys;
    index=left_node->num_keys;
    
    numright=right_node->num_keys;
    if(numleft>=numright){
    
        for(i=index;i<numright+1;i++){
        
            left_node->keys[i]=right_node->keys[j];
            left_node->records[i]=right_node->records[j];
            j++;
        
        }
        
        update_parent_nodes(right_node);
        left_node->next=right_node->next;
        
        right_node=NULL;
        
    }
  }

    return RC_OK;

}

RC delete_parent_nodes_inital(Btree_stat *stat,Btree *node,Value *key){
    
     int i=0,j;
    Btree *new_node, *left_node;
    if(node!=NULL){
    while (node->keys[i] != key->v.intV){
        i++;
    }
    if(node->num_keys>1){
    for (j=i; j < node->num_keys-1; j++){
        node->keys[j] = node->keys[j+1];
        node->records[j]=node->records[j+1];
        node->pointers[j+1]=node->pointers[j+2];
 
    }
    new_node=node->pointers[1];
    // node->pointers[j+1]=NULL;
     node->num_keys--;
    }

    else{
        new_node=node->parent;
        left_node=new_node->pointers[1];
        node->keys[i] =NULL;
        node->records[i].slot=0;
        node->records[i].page=0;
        node->pointers[i+1]=NULL;
        if(node->num_keys!=0){
        merge_nodes(stat,node->prev,node);
        }
    }
    delete_parent_nodes_inital(stat,node->parent,key);
   }
    return RC_OK;

}


RC update_parent_nodes_inital(Btree *child,Value *key){

    Btree *parent;
    int i=0,j;
   
    parent=child->parent;
    if(parent!=NULL){
        for(i=0;i<parent->num_keys;i++){
        if(parent->keys[i]==key->v.intV){
            
        parent->keys[i]=child->keys[0];
            
            }
        }
    
    update_parent_nodes_inital(parent,key);
    }

    return RC_OK;

}


RC deleteKey (BTreeHandle *tree, Value *key){

	Btree * root;
	Btree_stat *node;
	node=tree->mgmtData;
        root=node->mgmtData;
	Btree * key_leaf,*left_node;
    key_leaf = find_leaf(tree, key);
    left_node=key_leaf->prev;
    if (key_leaf != NULL) {
        
        if(key_leaf->prev==NULL) {
            
         delete_entry(tree, key_leaf, key);
      
         return RC_OK;
        
        }
        if(key_leaf->keys[0]==key->v.intV){
        
            delete_entry(tree, key_leaf, key);
            if(key_leaf->num_keys==0){
            
               left_node->next=key_leaf->next;
               delete_parent_nodes_inital(node,key_leaf,key);
               return RC_OK;
            }
            
            update_parent_nodes_inital(key_leaf,key);
            return RC_OK;
        
        }
        
        else{
        
        delete_entry(tree, key_leaf, key);
        
        if(key_leaf!=NULL){
             if(checkUnderflow(node,key_leaf)==true){
    
       
       if(left_node!=NULL){
       
           if(left_node->num_keys > split_node(node->order) && left_node->num_keys!=node->order){
           
               redistribute(node,left_node,key_leaf);
           
                     } 
           else {
           
               merge_nodes(node,left_node,key_leaf);
           
           }
       
               }
    
            }
        }
        
        return RC_OK;
        
       }
  
        
  
    
    }
    return RC_OK;
}




RC closeBtree(BTreeHandle* tree){
    Btree_stat *root;
    root=tree->mgmtData;
    shutdownBufferPool(root->fileInfo);
    free(root->fileInfo);
    free(root->mgmtData);
    tree->idxId=NULL;
    return RC_OK;

}

RC deleteBtree(char* idxId){
    
    destroyPageFile(idxId);
    return RC_OK;

}

RC shutdownIndexManager(){
    
    return RC_OK;

}
RC closeTreeScan(BT_ScanHandle* handle){

    
    free(handle->mgmtData);
    free(handle->tree);
    return RC_OK;
}
