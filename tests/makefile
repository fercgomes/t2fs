CC=gcc
SRC_FILES = $(wildcard *.c)
TARGETS = ${SRC_FILES:.c= }
CFLAGS =  -m32 -g

.PHONY: all compile run

all: compile run clean
	
run:
	cd ../;	echo "\nRUNNING TESTS:\n"; ./read_mbr_test; echo "\n===READ MBR TEST FINISHED===\n"; ./checksum_test; echo "\n===CHECKSUM TEST FINISHED===\n"; ./format2_test; echo "\n===FORMAT TEST FINISHED===\n"; echo "\nTESTS FINISHED\n";
	
compile:
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ read_mbr_test.c -lt2fs -o ../read_mbr_test -Wall
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ checksum_test.c -lt2fs -o ../checksum_test -Wall
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ format2_test.c -lt2fs -o ../format2_test -Wall


clean:
	rm -rf *.o;	cd ../;	rm $(TARGETS);
