CC=gcc
SRC_FILES = $(wildcard *.c)
TARGETS = ${SRC_FILES:.c= }
CFLAGS =  -m32 -g
SHELL=/bin/bash 

.PHONY: all compile run check

all: compile run clean check
	
run:
	@cd ../;\
	printf "\nRUNNING TESTS:\n";\
	printf "===READ MBR TEST BEGINS===\n"; ./read_mbr_test > ./tests/log/read_mbr_test.txt;\
	printf "===CHECKSUM TEST BEGINS===\n"; ./checksum_test > ./tests/log/checksum_test.txt;\
	printf "===FORMAT TEST BEGINS===\n"; ./format2_test > ./tests/log/format_test.txt;\
	printf "===MOUNT TEST BEGINS===\n"; ./mount_test > ./tests/log/mount_test.txt;\
	printf "===LIST TEST BEGINS===\n"; ./list_test > ./tests/log/list_test.txt;\
	printf "===FILENAME TEST BEGINS===\n"; ./filename_test > ./tests/log/filename_test.txt;\
	printf "===CREATE INODE TEST BEGINS===\n"; ./createinode_test > ./tests/log/createinode_test.txt;\
	printf "===BLOCKS TEST BEGINS===\n"; ./blocks_test > ./tests/log/blocks_test.txt;\
	printf "\nTESTS FINISHED\nCHECK THE LOGS AT tests/log\n";
	
compile:
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ read_mbr_test.c -lt2fs -o ../read_mbr_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ checksum_test.c -lt2fs -o ../checksum_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ format2_test.c -lt2fs -o ../format2_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ mount_test.c -lt2fs -o ../mount_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ list_test.c -lt2fs -o ../list_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ filename_test.c -lt2fs -o ../filename_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ createinode_test.c -lt2fs -o ../createinode_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ blocks_test.c -lt2fs -o ../blocks_test -Wall

check:
	@var=0;lines=($$(grep -iRl "NOT OK" ./log/));for x in $${lines[@]}; do if [ $$var = 0 ]; then printf "\nERRORS FOUND IN:\n"; fi; var=$$(($$var + 1)); printf "$$x\n"; done; if [ $$var = 0 ]; then printf "\nNO ERRORS WERE FOUND\nALL OK\n"; else printf "\nERRORS WERE FOUND\nCHECK THE LOGS AND CORRECT IT\n"; fi;
	
clean:
	@rm -rf *.o;	cd ../;	rm $(TARGETS);
