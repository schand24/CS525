SRC := \
	dberror.c \
	storage_mgr.c \
	buffer_mgr_stat.c \
	buffer_mgr.c \
	expr.c   \
	rm_serializer.c   \
	record_mgr.c   \
	btree_mgr.c  \
	test_assign4_1.c
	
	
OBJ := \
	dberror.o \
	storage_mgr.o \
	buffer_mgr_stat.o \
	buffer_mgr.o \
	expr.o   \
	rm_serializer.o   \
	record_mgr.o   \
	btree_mgr.o  \
	test_assign4_1.o
	

assign4: $(OBJ)
	gcc -o assign4 $?
	

%.o: %.c
	gcc -g -c $<

clean:
	rm -rf assign4 *.o
