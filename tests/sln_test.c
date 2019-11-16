
#include <stdio.h>
#include <t2fs.h>
#include <utils.h>

typedef struct s_partition PARTITION;

extern PARTITION part;

int main() {
	DIRENT2 dentry;
	BYTE goodname[51] = "ImAGoodName";
	BYTE goodname2[51] = "IamAHELLofaGOODname!!!\"'123\\azAZ@";
	BYTE badname[100] = "LLLLOOOOOONGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG";

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
	write2(fp, goodname, 51);

	printf("Closing file.\n");
	close2(fp);

	printf("creating symlink\n");
	sln2("symlink", "target");

	printf("Opening hlink.\n");
	fp = open2("symlink");

	printf("Reading from symlink\n");
	read2(fp, badname, 100);

	printf("From symlink read...\n%s\n", badname);

	printf("Closing symlink\n");
	close2(fp);

	printf("Deleting original file.\n");
	delete2("target");

	printf("Reading from symlink again\n");
	fp = open2("symlink");
	read2(fp, badname, 100);
	printf("From symlink read...\n%s\n", badname);

	printf("Closing symlink\n");
	close2(fp);

	printf("Deleting symlink\n");
	delete2("symlink");

	printf("Trying to open link.\n");
	fp = open2("symlink");

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
