ADO Assignment4 : Submission

Group Members : 
	Krutharth soni 		(A20340570)
	Tirth Patel             (A20320187)
	Monisha		        (A20320532)
	Saibabu chandramouli    (A20345775)



Assignment Implementations:

*This Assignment includes implementation of B+tree that  allows navigation through records, and inserting and deleting records.

	Implemented a B+ tree index and tested.


createdBtree, openBtree, closeBtree and deleteBtree functionalities are implemented for managing the b plus tree. Creating a b plus tree by creating a pagefule and 
pin it into a buffer pool. Empty nodes are created. Open the b plus tree using openBtree functionality by reading the tree from the files.

getNumNodes,getNumEntries,getKeyType functionalities are implemented to access informations about a b plus tree.


findKey implemented for finding the key value and its record id. For finding the key we are starting  from the root, we are looking for the leaf which may contain the particular key value. 

insertKey is implemented for inserting a tree value. Helper functions such as find_node_to_insert for finding which nodes to insert,Split_and_insert_parent for split and 
insert into parent,Split_and_insert for split node and insert, insert_into_parent for inserting into parent,find_node_parent for finding a parent node,split_node for
 calculate the node split position are implemented.
 
 deleteKey is implemented to delete a tree value. Helper functions such as merge_nodes for merging two nodes,update_parent_nodes for updating the parent nodes,find_leaf find a leaf searching the key value for deleting,delete_entry for deleting and entry from the b + tree,
 redistribute for redistributing the nodes after deleting and update_parent_node for updating the parent node are implemented.
 
 
 openTreeScan,nextEntry, closeTreeScan are implementing for scanning through all the entries in a b + tree. If there is no more entry in the b + tree the nextEntry is returning RC_IM_NO_MORE_ENTRIES.
 
 
 printBtree implemented for displaying the b plus tree. 
 

Compiling Instruction: 
We have implemented the following make files (makefile1) 

1.Makefile1 : This includes the test file test_assign4_1.c 

	Command to compile : make -f Makefile1
	Output file generated: assign4
	Command to Run : ./assign4


