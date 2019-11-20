
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
	
	int max_files = part.max_files_open;
	
	char filenames[max_files][51];
	char notwritten[51];
	sprintf(notwritten, "%d", max_files);

	int i;
	for (i = 0; i < max_files; i++) {
		sprintf(filenames[i], "%d", i);
	}
	
	FILE2 fHandler[max_files];
	for (i = 0; i < max_files; i++) {
		fHandler[i] = create2(filenames[i]);
		printf("Creating file %d: FID %d %s\n", i, fHandler[i], fHandler[i] >= 0 ? "OK" : "NOT OK");
	}
	
	printf("Closing file: %s\n", close2(fHandler[i-1]) == 0 ? "OK" : "NOT OK");
	fHandler[i-1] = create2(filenames[i-1]);
	printf("Creating file already written: %s\n", fHandler[i-1] >= 0 ? "OK" : "NOT OK");
	
	char deletedfile[51] = "6";
	
	printf("Closing file: %s\n", close2(fHandler[6]) == 0 ? "OK" : "NOT OK");
	printf("Deleting file %s: %s\n", deletedfile, delete2(deletedfile) == 0 ? "OK" : "NOT OK");
	printf("\tDeleting file again: %s\n", delete2(deletedfile) != 0 ? "OK" : "NOT OK");
	
	fHandler[6] = create2(deletedfile);
	printf("Recreating file %s: %s\n", deletedfile, fHandler[6]  >= 0 ? "OK" : "NOT OK"); 
	
	FILE2 hd1 = open2(deletedfile);
	FILE2 hd2 = open2(deletedfile);
	printf("Opening file %s: %s\n", deletedfile, hd1 >= 0 ? "OK" : "NOT OK");
	printf("Opening same file %s: %s\n", deletedfile, hd2 >= 0 ? "OK" : "NOT OK");
	
	printf("Closing first file: %s\n", close2(hd1) == 0 ? "OK" : "NOT OK");
	printf("Closing second file: %s\n", close2(hd2) == 0 ? "OK" : "NOT OK");
	
	printf("Trying to close already closed: %s\n", close2(hd1) != 0 ? "OK" : "NOT OK");
	
	for (i = 0; i < max_files; i++) {
		printf("Closing file %d: %s\n", i, close2(fHandler[i]) == 0 ? "OK" : "NOT OK");
	}	
	
	if(umount()) {
		printf("Error while unmounting 1: NOT OK\n");
		return -1;
	}

	return 0;
}
