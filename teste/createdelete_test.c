
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
	
	int activate_max_dentries_test = (max_files == part.max_dentries) ? 1 : 0;
	
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
	
	int deleted_id = 6;
	char deletedfile[51] = "6";
	
	printf("Closing file: %s\n", close2(fHandler[deleted_id]) == 0 ? "OK" : "NOT OK");
	printf("Deleting file %s: %s\n", deletedfile, delete2(deletedfile) == 0 ? "OK" : "NOT OK");
	printf("\tDeleting file again: %s\n", delete2(deletedfile) != 0 ? "OK" : "NOT OK");
	
	fHandler[deleted_id] = create2(deletedfile);
	printf("Recreating file %s: %s\n", deletedfile, fHandler[6]  >= 0 ? "OK" : "NOT OK"); 
	
	if (opendir2()) {
		printf("Error while openning dir: NOT OK\n");
		return -1;
	}
	
	i = 0;
	DIRENT2 dummydentry;
	while (!readdir2(&dummydentry)) {
		if (i < deleted_id || activate_max_dentries_test) {
			printf("Dentry name: %s - Index: %s %s\n", filenames[i], dummydentry.name, strcmp(filenames[i], dummydentry.name) == 0 ? "OK" : "NOT OK");
		} else {
			if (i < max_files-1 && !activate_max_dentries_test) {
				printf("Dentry name: %s - Index: %s %s\n", filenames[i+1], dummydentry.name, strcmp(filenames[i+1], dummydentry.name) == 0 ? "OK" : "NOT OK");
			} else if ( !activate_max_dentries_test ) {
				printf("Dentry name: %s - Index: %s %s\n", filenames[deleted_id], dummydentry.name, strcmp(filenames[deleted_id], dummydentry.name) == 0 ? "OK" : "NOT OK");
			}
		}			
		
		i++;
	}
	
	if (i != max_files) printf("Failed reading all files created in dir: NOT OK\n");
	else printf("Success reading all files in dir: OK\n");
	
	for (i = 0; i < max_files; i++) {
		printf("Closing file %d: %s\n", i, close2(fHandler[i]) == 0 ? "OK" : "NOT OK");
	}	
	
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
