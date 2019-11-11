CC=gcc
TARGETS = ${SRC_FILES:.c= }
CFLAGS =  -m32 -g

all:
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ hlink_test.c -lt2fs -o hlink_test -Wall