
#include <stdio.h>
#include <t2fs.h>

int main() {
	if(format2(1, 1)) {
		printf("Error while formatting 1.\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 1.\n");
		return -1;
	}
	
	
	
	if(umount()) {
		printf("Error while unmounting 2.\n");
		return -1;
	}

	return 0;
}
