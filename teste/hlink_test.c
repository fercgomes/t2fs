
#include <stdio.h>
#include <t2fs.h>
#include <utils.h>


int main() {
	DIRENT2 dirent;
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
	
	printf("Openning dir: %s\n", opendir2() ? "NOT OK" : "OK");

	FILE2 fp;
	printf("Creating new empty file: %s\n", (fp = create2("target")) < 0 ? "NOT OK" : "OK");

	printf("Writing to original file: %s\n", write2(fp, test_buffer, 100) == 100 ? "OK" : "NOT OK");

	printf("Closing file: %s\n", close2(fp) ? "NOT OK" : "OK");

	printf("Creating hlink: %s\n", hln2("link", "target") ? "NOT OK" : "OK");

	printf("Opening hlink: %s\n", (fp = open2("link")) < 0 ? "NOT OK" : "OK");

	printf("Reading from hlink: %s\n", read2(fp, test_buffer2, 100) == 100 ? "OK" : "NOT OK");

	printf("From hlink read: %s\n", strcmp(test_buffer, test_buffer2) == 0 ? "OK" : "NOT OK");

	printf("Closing hlink: %s\n", close2(fp) ? "NOT OK" : "OK");

	printf("Deleting original file %s\n", delete2("target") ? "NOT OK" : "OK");

	printf("Opening hlink: %s\n", (fp = open2("link")) < 0 ? "NOT OK" : "OK");
	
	strcpy(test_buffer2, "ASDYHBKJHNAMKDAHNJS");
	
	printf("Reading from hlin: %s\n", read2(fp, test_buffer2, 100) == 100 ? "OK" : "NOT OK");
	printf("Data from hlink read: %s\n", strcmp(test_buffer, test_buffer2) == 0 ? "OK" : "NOT OK");

	printf("Closing hlink: %s\n", close2(fp) ? "NOT OK" : "OK");

	printf("Deleting hlink: %s\n", delete2("link") ? "NOT OK" : "OK");

	printf("Trying to open deleted link: %s\n", (fp = open2("link")) < 0 ? "OK" : "NOT OK");

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
