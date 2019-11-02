CC=gcc
SRC_FILES = $(wildcard *.c)
TARGETS = ${SRC_FILES:.c= }
CFLAGS =  -m32 -g


.PHONY: all compile run check

all: compile run clean check
	
run:
	@cd ../;\
	echo "\nRUNNING TESTS:";\
	echo "===READ MBR TEST BEGINS==="; ./read_mbr_test > ./tests/log/read_mbr_test.txt;\
	echo "===CHECKSUM TEST BEGINS==="; ./checksum_test > ./tests/log/checksum_test.txt;\
	echo "===FORMAT TEST BEGINS==="; ./format2_test > ./tests/log/format_test.txt;\
	echo "===MOUNT TEST BEGINS==="; ./mount_test > ./tests/log/mount_test.txt;\
	echo "===LIST TEST BEGINS==="; ./list_test > ./tests/log/list_test.txt;\
	echo "===FILENAME TEST BEGINS==="; ./filename_test > ./tests/log/filename_test.txt;\
	echo "===CREATE INODE TEST BEGINS==="; ./createinode_test > ./tests/log/createinode_test.txt;\
	echo "\nTESTS FINISHED\nCHECK THE LOGS AT tests/log";
	
compile:
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ read_mbr_test.c -lt2fs -o ../read_mbr_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ checksum_test.c -lt2fs -o ../checksum_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ format2_test.c -lt2fs -o ../format2_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ mount_test.c -lt2fs -o ../mount_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ list_test.c -lt2fs -o ../list_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ filename_test.c -lt2fs -o ../filename_test -Wall
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ createinode_test.c -lt2fs -o ../createinode_test -Wall

check:
	@echo "\n=== CHECKING FOR \"NOT OK\"==="
	@var=0; grep -iRl "NOT OK" ./log/ | while read -r line; do\
		if [ $$var = 0 ]; then\
			echo "ERRORS FOUND:\n";\
			var=1;\
		fi;\
		echo "$$line";\
		\
	done;\
	if [ $$var = 0 ]; then echo "NO ERRORS WERE FOUND\n";\
	fi;
	
		
clean:
	@rm -rf *.o;	cd ../;	rm $(TARGETS);
