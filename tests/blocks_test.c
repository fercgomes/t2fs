
#include <stdio.h>
#include <t2fs.h>
#include <utils.h>

int localize_freeblock(); // Retorna negativo se erro, zero se não achou ou o ID do bloco (positivo)
int allocate_freeblock(int id); // Retorna negativo se erro, zero se não achou ou o ID do bloco (positivo)
int deallocate_block(int id); // Retorna zero se sucesso, outro número se fracasso (erro ao desalocar bloco não alocado)
unsigned int blockid_to_sector(int id); // Retorna -1 se fracasso, ID do bloco se sucesso.
BLOCKBUFFER new_block_buffer(); // Retorna um buffer de bloco (USE WISELY)
BLOCKBUFFER free_block_buffer(BLOCKBUFFER buffer); // Retorna NULL se sucesso. Segmentation fault se falha lol (USE WISELY)
int write_new_block(BLOCKBUFFER block, int* block_id); // Retorna zero se sucesso e guarda o id do novo bloco em block_id, outro número se falha.
int write_block(BLOCKBUFFER block, int block_id); // Retorna zero se sucesso, outro número se falha (não escreve bloco não alocado)
int load_block(BLOCKBUFFER block_buffer, int id); // Retorna zero se sucesso e conteúdo no block_buffer, retorna outro número se falha.

int main() {
	if(format2(1, 4)) {
		printf("Error while formatting 1.\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 1.\n");
		return -1;
	}
	
	int blockId = localize_freeblock();
	printf("Localize block: %s\n", blockId == 1 ? "OK" : "NOT OK"); 
	printf("Allocate block: %s\n", allocate_freeblock(blockId) == 0 ? "OK" : "NOT OK");
	printf("Free block: %s\n", deallocate_block(blockId) == 0 ? "OK" : "NOT OK");
	printf("Failed free: %s\n", deallocate_block(blockId) == -1 ? "OK" : "NOT OK");
	
	BLOCKBUFFER fakeblock[200];
	int ids[200] = {0};
	int max_id = 169;
	int i;
	for (i = 0; i < 200; i++) {
		fakeblock[i] = new_block_buffer();
		if (!fakeblock[i])
			printf("Failed to allocate a blockbuffer: NOT OK\n");
	}
	
	for (i = 0; i < 200; i++) {
		fakeblock[i][SECTOR_SIZE - i - 1] = (BYTE) i;
		fakeblock[i][SECTOR_SIZE*2 - i - 1] = (BYTE) i+1;
		fakeblock[i][SECTOR_SIZE*3 - i - 1] = (BYTE) i+2;
		fakeblock[i][SECTOR_SIZE*4 - i - 1] = (BYTE) i+3;
		
		printf("Writing new block: %d %s - ", i, (write_new_block(fakeblock[i], &ids[i]) == 0 || i >= max_id-1) ? "OK" : "NOT OK");
		printf("Checking id: %d %s\n", ids[i], (ids[i] == i+1 || ids[i] == 0) ? "OK" : "NOT OK");
	}
		
	int correct_data;
	int loaded;
	for (i = 0; i < 200; i++) {
		loaded = 1;
		if (load_block(fakeblock[i], ids[i]) && i < max_id-1) {
			printf("Failed to load valid block: ID %d\n\tMax id: %d NOT OK\n", i+1, max_id);
			loaded = 0;
		} else {
			printf("Load function at id %d: OK\n", ids[i]);
		}
		
		if (loaded) {
			correct_data = fakeblock[i][SECTOR_SIZE - i - 1] == (BYTE) i \
						&& fakeblock[i][SECTOR_SIZE*2 - i - 1] == (BYTE) i+1 \
						&& fakeblock[i][SECTOR_SIZE*3 - i - 1] == (BYTE) i+2 \
						&& fakeblock[i][SECTOR_SIZE*4 - i - 1] == (BYTE) i+3;
			printf("Loaded block %d - Data: %s\n", ids[i], correct_data ? "OK" : "NOT OK");
		}
	}
	
	int removed_block = 143;
	
	printf("\n=== Removing block %d: %s\n\n", ids[i], deallocate_block(ids[removed_block]) == 0 ? "OK" : "NOT OK");
	
	for (i = 0; i < 200; i++) {
		fakeblock[i][SECTOR_SIZE - i - 12] = (BYTE) i+1;
		fakeblock[i][SECTOR_SIZE*2 - i - 12] = (BYTE) i+2;
		fakeblock[i][SECTOR_SIZE*3 - i - 12] = (BYTE) i+3;
		fakeblock[i][SECTOR_SIZE*4 - i - 12] = (BYTE) i+4;

		if (i != removed_block) printf("Writing block %d: %s\n", ids[i], (write_block(fakeblock[i], ids[i]) == 0 || i >= max_id-1) ? "OK" : "NOT OK");
		else printf("Writing deallocated block %d: %s\n", ids[i], write_block(fakeblock[i], ids[i]) != 0 ? "OK" : "NOT OK");
	}
	
	for (i = 0; i < 200; i++) {	
		if (i != removed_block) {
			loaded = 1;
			if (load_block(fakeblock[i], ids[i]) && i < max_id-1) {
				printf("Failed to load valid block: ID %d\n\tMax id: %d NOT OK\n", i+1, max_id);
				loaded = 0;
			} else {
				printf("Load function at id %d: OK\n", ids[i]);
			}
			
			if (loaded) {
				correct_data = fakeblock[i][SECTOR_SIZE - i - 1] == (BYTE) i \
							&& fakeblock[i][SECTOR_SIZE*2 - i - 1] == (BYTE) i+1 \
							&& fakeblock[i][SECTOR_SIZE*3 - i - 1] == (BYTE) i+2 \
							&& fakeblock[i][SECTOR_SIZE*4 - i - 1] == (BYTE) i+3 \
							&& fakeblock[i][SECTOR_SIZE - i - 12] == (BYTE) i+1 \
							&& fakeblock[i][SECTOR_SIZE*2 - i - 12] == (BYTE) i+2 \
							&& fakeblock[i][SECTOR_SIZE*3 - i - 12] == (BYTE) i+3 \
							&& fakeblock[i][SECTOR_SIZE*4 - i - 12] == (BYTE) i+4;
							
				printf("Loaded block %d - Data: %s\n", ids[i], correct_data ? "OK" : "NOT OK");
			}
		}
		else printf("Loading deallocated block %d: %s\n", ids[i], load_block(fakeblock[i], ids[i]) != 0 ? "OK" : "NOT OK");
	}
	
	for (i = 0; i < 200; i++) {
		fakeblock[i] = free_block_buffer(fakeblock[i]);
		if (fakeblock[i])
			printf("Failed to deallocate a blockbuffer: NOT OK\n");
	}
	
	if(umount()) {
		printf("Error while unmounting 1.\n");
		return -1;
	}
	
	
	
	return 0;
}
