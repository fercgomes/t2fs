#
# Makefile ESQUELETO
#
# DEVE ter uma regra "all" para geração da biblioteca
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
# 

CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
TEST_DIR=./tests

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst %.c,%.o,$(wildcard $(SRC_DIR)/*.c))
HEADERS = $(wildcard include/*.h)

TARGET = $(LIB_DIR)/libt2fs.a
MOODLE_NAME = entrega.tar.gz

.PHONY: all t2fs pack tests

all: t2fs

t2fs: $(OBJECTS)
	@ar crs $(TARGET) $(foreach var, $(OBJECTS), $(BIN_DIR)/$(notdir $(var))) $(LIB_DIR)/apidisk.o $(LIB_DIR)/bitmap2.o $(LIB_DIR)/support.o

# Generic rules
%.o: %.c $(HEADERS)
	@$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $(BIN_DIR)/$(notdir $@) -Wall
	
clean:
	@rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(SRC_DIR)/*.o $(INC_DIR)/*~ *~

# Compile tests
tests: t2fs
	@$(MAKE) -C $(TEST_DIR)
	
pack:
	@echo "Preparando para entrega..."
	tar -zcvf $(MOODLE_NAME) bin/ exemplos/ include/ lib/ src/ makefile t2fs_disk.dat
