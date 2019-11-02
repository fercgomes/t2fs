
#include <stdio.h>
#include <t2fs.h>
#include <utils.h>

int localize_freeinode(); // Retorna negativo se erro, zero se não achou ou o ID do bloco (positivo)
int allocate_inode(int id);
unsigned int inodeid_to_sector(int id);
unsigned int inodeid_in_sector(int id);
int write_new_inode(DIRENT2* dentry);
int write_inode(DIRENT2 dentry, INODE2 inode);
int load_inode(DIRENT2 dentry, INODE2* inode);
int delete_inode(INODE2* inode);

int main() {
	if(format2(1, 20)) {
		printf("Error while formatting 1.\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 1.\n");
		return -1;
	}
	
	int inodeID = localize_freeinode();
	printf("Localize inode: %s\n", inodeID == 1 ? "OK" : "NOT OK"); 
	
	if(umount()) {
		printf("Error while unmounting 1.\n");
		return -1;
	}
	
	if(format2(1, 20)) {
		printf("Error while formatting 2.\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 2.\n");
		return -1;
	}
	
	DIRENT2 fakedentry[700];
	int max_inodes = 639;
	int i;
	int res;
	for (i = 0; i < 5; i++) {
		res = write_new_inode(&fakedentry[i]);
		printf("Creating inode:\tID: %d %s Written: %s\n", fakedentry[i].inodeNumber, fakedentry[i].inodeNumber == i+1 ? "OK" : "NOT OK",\
														  !res ? "OK" : "NOT OK");
	}
	
	INODE2 dummyinode;
	DIRENT2 invaliddentry;
	invaliddentry.inodeNumber = 6;
	printf("Loading missing inode: %s\n", load_inode(invaliddentry, &dummyinode) ? "OK" : "NOT OK");
	printf("Loading real inode: %s\n", load_inode(fakedentry[3], &dummyinode) ? "NOT OK" : "OK");
	print_inode(dummyinode);
	
	if(umount()) {
		printf("Error while unmounting 2.\n");
		return -1;
	}
	
	if(format2(1, 4)) {
		printf("Error while formatting 3.\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 3.\n");
		return -1;
	}
	
	for (i = 0; i < 700; i++) {
		res = write_new_inode(&fakedentry[i]);
		printf("Creating inode:\tID: %d %s Written: %s\n", fakedentry[i].inodeNumber, fakedentry[i].inodeNumber == i+1 || i >= max_inodes? "OK" : "NOT OK",\
														  (res && i < max_inodes) ? "NOT OK" : "OK");
	}
	
	for (i = 0; i < 700; i++) {
		res = load_inode(fakedentry[3], &dummyinode);
		printf("Loading real inode: %s - Data: %s - ID: %d %s\n", (res && i < max_inodes) ? "NOT OK" : "OK", dummyinode.blocksFileSize == 0 || i >= max_inodes ? "OK" : "NOT OK",\
													fakedentry[i].inodeNumber, fakedentry[i].inodeNumber == i+1 || i >= max_inodes? "OK" : "NOT OK");
	}
	
	for (i = 0; i < 700; i++) {
		dummyinode.blocksFileSize = i;
		printf("Writing inode back: %s\n", write_inode(fakedentry[i], dummyinode) && i < max_inodes? "NOT OK" : "OK");
	}
	
	for (i = 0; i < 700; i++) {
		res = load_inode(fakedentry[i], &dummyinode);
		printf("Loading real inode: %s - Data: %d %s - ID: %d %s\n", (res && i < max_inodes) ? "NOT OK" : "OK", dummyinode.blocksFileSize,\
																	dummyinode.blocksFileSize == i || i >= max_inodes ? "OK" : "NOT OK",\
																	fakedentry[i].inodeNumber, fakedentry[i].inodeNumber == i+1 || i >= max_inodes ? "OK" : "NOT OK");
	}
	
	if(umount()) {
		printf("Error while unmounting 3.\n");
		return -1;
	}
	
	return 0;
}
