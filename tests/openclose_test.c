
#include <stdio.h>
#include <t2fs.h>

typedef struct s_partition PARTITION;

extern PARTITION part;

int main() {
	if (format2(1, 1)) {
		printf("Error while formatting 1: NOT OK\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 1: NOT OK\n");
		return -1;
	}
	
	if (opendir2()) {
		printf("Error while openning dir: NOT OK\n");
		return -1;
	}
	
	int max_files = part.max_dentries;
	
	char filenames[max_files][51];
	char notwritten[51];
	sprintf(notwritten, "%d", max_files);

	int i;
	for (i = 0; i < max_files; i++) {
		sprintf(filenames[i], "%d", i);
	}
	
	for (i = 0; i < max_files; i++) {
		printf("Creating file %d: %s\n", i, create2(filenames[i]) == 0 ? "OK" : "NOT OK");
	}
	printf("Creating file withouth space: %s\n", create2(notwritten) != 0 ? "OK" : "NOT OK");
	printf("Creating file already written: %s\n", create2(filenames[i-1]) == 0 ? "OK" : "NOT OK");
	
	char deletedfile[51] = "6";
	
	printf("Deleting file %s: %s\n", deletedfile, delete2(deletedfile) == 0 ? "OK" : "NOT OK");
	printf("\tDeleting file again: %s\n", delete2(deletedfile) != 0 ? "OK" : "NOT OK");
	printf("Recreating file %s: %s\n", deletedfile, create2(deletedfile) == 0 ? "OK" : "NOT OK"); 
	
	FILE2 hd1 = open2(deletedfile);
	FILE2 hd2 = open2(deletedfile);
	printf("Opening file %s: %s\n", deletedfile, hd1 >= 0 ? "OK" : "NOT OK");
	printf("Opening same file %s: %s\n", deletedfile, hd2 >= 0 ? "OK" : "NOT OK");
	
	printf("Closing first file: %s\n", close2(hd1) == 0 ? "OK" : "NOT OK");
	printf("Closing second file: %s\n", close2(hd2) == 0 ? "OK" : "NOT OK");
	
	printf("Trying to close already closed: %s\n", close2(hd1) != 0 ? "OK" : "NOT OK");
	
	if (closedir2()) {
		printf("Error while closing dir: NOT OK\n");
		return -1;
	}
	
	if(umount()) {
		printf("Error while unmounting 1: NOT OK\n");
		return -1;
	}

	return 0;
}
