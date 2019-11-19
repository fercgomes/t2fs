
#include <stdio.h>
#include <t2fs.h>

int load_superblock(int partition, SUPERBLOCK* spb);
void print_superblock(SUPERBLOCK spb);
int is_superblock(SUPERBLOCK spb);

int main() {
	SUPERBLOCK spb;
	
	format2(1, 15);
	
	printf("\nLoading and reading correct superblock:\n");
	load_superblock(1, &spb);
	print_superblock(spb);
	
	printf("\tValid superblock? %s\n", is_superblock(spb) ? "OK" : "NOT OK");
	
	SUPERBLOCK spb2;
	printf("Testing false superblock: %s\n", is_superblock(spb2) ? "NOT OK" : "OK");
	
	return 0;
}
