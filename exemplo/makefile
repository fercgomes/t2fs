CC=gcc
SRC_FILES = $(wildcard *.c)
TARGETS = ${SRC_FILES:.c= }
CFLAGS =  -m32 -g

all: compile run clean

compile:
	@$(CC) $(CFLAGS) -I ../include/ -L ../lib/ t2shell.c -lt2fs -o ../t2shell -Wall

run:
	cd ..; ./t2shell
   
clean:
	rm ../t2shell
