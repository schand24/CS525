ADO Assignment2 : Submission

Group Members : 
	Krutharth soni 		(A20340570)
	Tirth Patel             (A20320187)
	Monisha		        (A20320532)
	Saibabu chandramouli    (A20345775)



Assignment Implementations:

*This first Assignment includes implementation of Buffer Manager which is capable of all file read write operations.

*Implements empty 
	Creating dummy pages,
	checking dummy pages operations 
	pin operation(to pin a page from disk to buffer manager based on FIFO or LRU algorithm)
	unPin operaton to unpin the bufferd page
	makedirty operaton to check if the page is modified by the user in the buffer;


*Implemented additional error codes returned such as 

define RC_BM_PAGE_NOT_FOUND 100
#define RC_BM_PAGE_FOUND 101
#define RC_BM_PAGE_INSERTED 102
#define RC_BM_PAGE_NOT_INSERTED 103


 which are returned depending on the errors occured with in our buffer manager application.

*Test cases provided in the class test_assign2_1.c is executed successfully except few mistakes in LRU logic.



Compiling Instruction: 
We have implemented the following make files (makefile1) 

1.Makefile1 : This includes the test file test_assign2_1.c 

	Command to compile : make -f Makefile1
	Output file generated: assign2
	Command to Run : ./assign2



