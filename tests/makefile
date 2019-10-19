CC=gcc
SRC_FILES = $(wildcard *.c)
TARGETS = ${SRC_FILES:.c= }
CFLAGS =  -m32 -g

.PHONY: all compile run

all: compile run clean
	
run:
	cd ../;	echo "\nRunning tests:\n"; ./read_mbr_test; echo "\nTests Finished\n";
	
compile:
	$(CC) $(CFLAGS) -I ../include/ -L ../lib/ read_mbr_test.c -lt2fs -o ../read_mbr_test

clean:
	rm -rf *.o;	cd ../;	rm $(TARGETS);
