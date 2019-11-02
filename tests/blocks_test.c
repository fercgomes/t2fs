
#include <stdio.h>
#include <t2fs.h>
#include <utils.h>

int localize_freeblock(); // Retorna negativo se erro, zero se não achou ou o ID do bloco (positivo)
int allocate_freeblock(int id); // Retorna negativo se erro, zero se não achou ou o ID do bloco (positivo)

int main() {
	if(format2(1, 20)) {
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
	
	if(umount()) {
		printf("Error while unmounting 1.\n");
		return -1;
	}
	
	return 0;
}
