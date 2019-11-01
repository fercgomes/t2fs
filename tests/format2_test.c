
#include "t2fs.h"
#include <stdio.h>

int load_superblock(int partition, SUPERBLOCK* spb);
void print_superblock(SUPERBLOCK spb);
int is_superblock(SUPERBLOCK spb);

int main() {
	SUPERBLOCK spb;
	
	format2(1, 15);
	
	printf("\nLoading and reading superblock:\n");
	load_superblock(1, &spb);
	print_superblock(spb);
	
	printf("\tValid superblock? %s\n", is_superblock ? "True" : "False");
	
	return 0;
}
