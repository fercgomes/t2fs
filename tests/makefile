CC=gcc
SRC_FILES = $(wildcard *.c)
TARGETS = ${SRC_FILES:.c= }
CFLAGS =  -m32 -g

.PHONY: all compile run

all: compile run clean
	
run:
	cd ../;	echo "\nRUNNING TESTS:\n"; echo "\n===READ MBR TEST BEGINS===\n"; ./read_mbr_test > ./tests/log/read_mbr_test.txt; echo "\n===CHECKSUM TEST BEGINS===\n"; ./checksum_test > ./tests/log/checksum_test.txt; echo "\n===FORMAT TEST BEGINS===\n"; ./format2_test > ./tests/log/format_test.txt; echo "\n===MOUNT TEST BEGINS===\n"; ./mount_test > ./tests/log/mount_test.txt; echo "\n===LIST TEST BEGINS===\n"; ./list_test > ./tests/log/list_test.txt; echo "\n===FILENAME TEST BEGINS===\n"; ./filename_test > ./tests/log/filename_test.txt; echo "\n===CREATE INODE TEST BEGINS===\n"; ./createinode_test > ./tests/log/createinode_test.txt; echo "\nTESTS FINISHED\nCHECK THE LOGS AT tests/log";
	
compile:
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ read_mbr_test.c -lt2fs -o ../read_mbr_test -Wall
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ checksum_test.c -lt2fs -o ../checksum_test -Wall
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ format2_test.c -lt2fs -o ../format2_test -Wall
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ mount_test.c -lt2fs -o ../mount_test -Wall
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ list_test.c -lt2fs -o ../list_test -Wall
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ filename_test.c -lt2fs -o ../filename_test -Wall
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ createinode_test.c -lt2fs -o ../createinode_test -Wall


clean:
	rm -rf *.o;	cd ../;	rm $(TARGETS);
