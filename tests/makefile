CC=gcc
SRC_FILES = $(wildcard *.c)
TARGETS = ${SRC_FILES:.c= }
CFLAGS =  -m32 -g

.PHONY: all compile run

all: compile run clean
	
run:
	cd ../;	echo "\nRUNNING TESTS:\n"; echo "\n===READ MBR TEST BEGINS===\n"; ./read_mbr_test; echo "\n===CHECKSUM TEST BEGINS===\n"; ./checksum_test; echo "\n===FORMAT TEST BEGINS===\n"; ./format2_test; echo "\n===LIST TEST BEGINS===\n"; ./list_test; echo "\n===LIST TEST BEGINS===\n"; ./filename_test; echo "\nTESTS FINISHED\n";
	
compile:
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ read_mbr_test.c -lt2fs -o ../read_mbr_test -Wall
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ checksum_test.c -lt2fs -o ../checksum_test -Wall
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ format2_test.c -lt2fs -o ../format2_test -Wall
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ list_test.c -lt2fs -o ../list_test -Wall
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ filename_test.c -lt2fs -o ../filename_test -Wall

clean:
	rm -rf *.o;	cd ../;	rm $(TARGETS);
