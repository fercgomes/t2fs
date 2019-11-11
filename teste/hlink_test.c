
#include <stdio.h>
#include <t2fs.h>
#include <utils.h>

typedef struct s_partition PARTITION;

extern PARTITION part;

int main() {
	DIRENT2 dentry;
	char test_buffer[100] = "Du gamla du fria du fjallhoga nord. Du tysta du gladjerika skjona.";
	char test_buffer2[100];

	if (format2(1, 1)) {
		printf("Error while formatting 1: NOT OK\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 1: NOT OK\n");
		return -1;
	}
	opendir2();

	printf("Creating new empty file.\n");
	FILE2 fp = create2("target");

	printf("Writing to original file.\n");
	write2(fp, test_buffer, 100);

	printf("Closing file.\n");
	close2(fp);

	printf("creating hlink\n");
	hln2("link", "target");

	printf("Opening hlink.\n");
	fp = open2("link");

	printf("Reading from hlink\n");
	read2(fp, test_buffer2, 100);

	printf("From hlink read...\n%s\n", test_buffer2);

	printf("Closing hlink\n");
	close2(fp);

	printf("Deleting original file.\n");
	delete2("target");

	printf("Reading from hlink again\n");
	fp = open2("link");
	read2(fp, test_buffer2, 100);
	printf("From hlink read...\n%s\n", test_buffer2);

	printf("Closing hlink\n");
	close2(fp);

	printf("Deleting hlink\n");
	delete2("link");

	printf("Trying to open link.\n");
	fp = open2("link");

	while(!readdir2(&dentry)) {
		printf("====================\n");
		printf("Name: %s\n", dentry.name);
		printf("inode: %d\n", dentry.inodeNumber);
		printf("Type: %d\n", dentry.TypeVal);
	}
	
	closedir2();
	if(umount()) {
		printf("Error while unmounting 1: NOT OK\n");
		return -1;
	}

	return 0;
}
