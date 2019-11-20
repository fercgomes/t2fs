
#include <stdio.h>
#include <t2fs.h>
#include <utils.h>

typedef struct s_partition PARTITION;

extern PARTITION part;

int main() {
	DIRENT2 dirent;
	char buffer[100] = "ImAGoodName";
	char buffer2[100] = "IamAHELLofaGOODname!!!\"'123\\azAZ@";
	char buffer3[100] = "LLLLOOOOOONGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG";

	if (format2(1, 1)) {
		printf("Error while formatting 1: NOT OK\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 1: NOT OK\n");
		return -1;
	}
	opendir2();
	FILE2 fp;
	printf("Creating new empty file: %s\n", (fp = create2("target")) < 0 ? "NOT OK" : "OK");

	printf("Writing to original file: %s\n", write2(fp, buffer, 100) == 100 ? "OK" : "NOT OK");

	printf("Closing file: %s\n", close2(fp) ? "NOT OK" : "OK");

	printf("Creating symlink: %s\n", sln2("symlink", "target") ? "NOT OK" : "OK");

	printf("Opening symlink: %s\n", (fp = open2("symlink")) < 0 ? "NOT OK" : "OK");

	printf("Reading from symlink: %s\n", read2(fp, buffer2, 100) == 100 ? "OK" : "NOT OK");

	printf("From symlink read: %s\n", strcmp(buffer2, buffer) == 0 ? "OK" : "NOT OK");

	printf("Closing symlink: %s\n", close2(fp) ? "NOT OK" : "OK");

	printf("Deleting original file: %s\n", delete2("target") ? "NOT OK" : "OK");

	printf("Loading symlink again: %s\n", (fp = open2("symlink")) < 0 ? "OK" : "OK");
	printf("Reading symlink: %s\n", read2(fp, buffer3, 100) == 100 ? "NOT OK" : "OK");
	printf("From symlink read: %s\n", strcmp(buffer, buffer3) == 0 ? "NOT OK" : "OK");

	printf("Closing symlink: %s\n", close2(fp) ? "OK" : " NOT OK");

	printf("Deleting symlink: %s\n", delete2("symlink") ? "NOT OK" : "OK");

	printf("Trying to open link : %s\n", (fp = open2("symlink")) < 0 ? "OK" : "NOT OK");

	while(!readdir2(&dirent)) {
		printf("====================\n");
		printf("Name: %s\n", dirent.name);
		printf("Size: %d\n", dirent.fileSize);
		printf("Type: %d\n", dirent.fileType);
	}
	
	printf("Closing dir: %s\n", closedir2() ? "NOT OK" : "OK");
	
	if(umount()) {
		printf("Error while unmounting 1: NOT OK\n");
		return -1;
	}

	return 0;
}
