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

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst %.c,%.o,$(wildcard $(SRC_DIR)/*.c))
HEADERS = $(wildcard include/*.h)

TARGET = $(LIB_DIR)/libt2fs.a
MOODLE_NAME = entrega.tar.gz

.PHONY: all t2fs pack

all: t2fs

t2fs: $(OBJECTS)
	ar crs $(TARGET) $(OBJECTS) $(LIB_DIR)/apidisk.o $(LIB_DIR)/bitmap2.o

# Generic rules
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@
	
clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(SRC_DIR)/*.o $(INC_DIR)/*~ *~

pack:
	@echo "Preparando para entrega..."
	tar -zcvf $(MOODLE_NAME) bin/ exemplos/ include/ lib/ src/ testes/ Makefile