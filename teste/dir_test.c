
#include <stdio.h>
#include <t2fs.h>

BLOCKBUFFER new_block_buffer();
BLOCKBUFFER free_block_buffer(BLOCKBUFFER buffer);
int append_block_to_inode(INODE2* inode, BLOCKBUFFER block); // Escreve um bloco na posição designada. Retorna zero se sucesso, outro número se falha - Usada para writes
int load_inode_block(INODE2 inode, BLOCKBUFFER block_buffer, unsigned int block_pos, unsigned int* block_id); // Carrega um dado bloco do inode. Retorna zero se sucesso, outro número se falha. - Usada para reads
int write_inode_block(INODE2* inode, BLOCKBUFFER block, unsigned int block_pos); // Escreve um bloco na posição designada. Retorna zero se sucesso, outro número se falha - Usada para writes

int search_file_in_dir(char* filename, DENTRY2* dentry, int* dentry_block, int* dentry_pos); // Busca uma dentry com o dado nome e retorna no ponteiro para dentry. Retorna zero se sucesso, outro número se fracasso.
int write_to_invalid_dentry_in_dir(DENTRY2 dentry); // Busca uma dentry inválida no diretório e escreve sobre ela. Retorna zero se sucesso, outro número se fracasso. (NÃO USAR)
int write_dentry_to_dir(DENTRY2 dentry); // Escreve uma dentry no diretório. Retorna zero se sucesso, outro número se fracasso.

typedef struct s_partition PARTITION;
typedef struct s_thedir THEDIR;

extern PARTITION part;
extern THEDIR* thedir;

int main() {
	if(format2(1, 1)) {
		printf("Error while formatting 1.\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 1.\n");
		return -1;
	}
	
	int max_blocks = (part.max_block_id-1);
	int max_dirs = part.dirs_in_block*max_blocks;
	BLOCKBUFFER block_buffer[max_blocks];
	
	DENTRY2 dentry[max_dirs];
	DENTRY2 dummydentry;
	DIRENT2 dummydirent;
	
	int i;
	for (i = 0; i < max_blocks; i++) {
		block_buffer[i] = new_block_buffer();
		if (!block_buffer[i]) printf("Failed allocating block buffer 1: NOT OK\n");
	}
	
	for (i = 0; i < max_dirs; i++) {
		sprintf(dentry[i].name, "%d", i);
		printf("%s\n", dentry[i].name);
		dentry[i].TypeVal = 0x01;
		memcpy((void*)&block_buffer[(i/part.dirs_in_block)][(i%part.dirs_in_block)*sizeof(DENTRY2)], (void*)&dentry[i], sizeof(DENTRY2));
	}
	
	printf("Openning dir: %s\n", opendir2() == 0 ? "OK" : "NOT OK");
	
	int last_block = 0;
	i = 0;
	printf("%d\n", thedir->inode.blocksFileSize);
	while(!append_block_to_inode(&(thedir->inode), block_buffer[i])) i++;
	last_block = i;
	
	if (last_block < 600) printf("Error writing inode blocks: NOT OK\n\tLast written: %d\n", last_block);
	
	thedir->inode.bytesFileSize = last_block*SECTOR_SIZE;
	
	int dummyint1;
	int dummyint2;
	printf("Searching for filename: %s\n", search_file_in_dir("3", &dummydentry, &dummyint1, &dummyint2) == 0 ? "OK" : "NOT OK");
	printf("\tChecking for filenames: Exp: %s Read: %s Result: %s\n", "3", dummydentry.name, strcmp("3", dummydentry.name) == 0 ? "OK" : "NOT OK");
	
	for (i = 0; i < last_block*part.dirs_in_block; i++) {
		printf("Reading dir: %s\n", readdir2(&dummydirent) == 0 ? "OK" : "NOT OK");
		printf("Checking data - Exp: %s Read: %s Result: %s\n", dentry[i].name, dummydirent.name, strcmp(dentry[i].name, dummydirent.name) == 0 ? "OK" : "NOT OK");
	}
	
	printf("Closing dir: %s\n", closedir2() == 0 ? "OK" : "NOT OK");
	
	if(umount()) {
		printf("Error while unmounting 2.\n");
		return -1;
	}
	for (i = 0; i < max_blocks; i++) block_buffer[i] = free_block_buffer(block_buffer[i]);
	
	printf("\n\n");
	
	if(format2(1, 2)) {
		printf("Error while formatting 2.\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 2.\n");
		return -1;
	}

	if (opendir2()) printf("Failed to open dir: NOT OK\n");
	
	dummydentry.TypeVal = 0x01;
	for (i = 0; i < part.max_dentries; i++) {
		printf("Writing dentry to dir: %d %s\n", i, write_dentry_to_dir(dummydentry) == 0 ? "OK" : "NOT OK");
	}
	
	printf("Full dir write: %s\n", write_dentry_to_dir(dummydentry) != 0 ? "OK" : "NOT OK");
	
	dummydentry.TypeVal = 0x0;
	
	unsigned int bid;
	
	BLOCKBUFFER bbuffer = new_block_buffer();
	if (!bbuffer) {
		printf("Buffer couldn't be created: NOT OK\n");
		return -1;
	}
	
	if (!load_inode_block(thedir->inode, bbuffer, (unsigned int)1, &bid)) {
		memcpy((void*)bbuffer, (void*)&dummydentry, sizeof(DENTRY2));
		
		if (write_inode_block(&(thedir->inode), bbuffer, bid)) printf("Couldn't write block to inode: NOT OK\n");
		
		dummydentry.TypeVal = 0x1;
		
		printf("Writing dentry to dir full dir: %d %s\n", i, write_dentry_to_dir(dummydentry) == 0 ? "OK" : "NOT OK");

	} else {
		printf("Couldn't load block: NOT OK\n");
	}
	
	if (closedir2()) printf("Failed to close dir: NOT OK\n");
	
	
	if(umount()) {
		printf("Error while unmounting 2.\n");
		return -1;
	}
	
	return 0;
}
