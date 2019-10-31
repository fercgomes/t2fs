
#include "t2fs.h"
#include <stdio.h>

int main() {
	int vals[5] = {5, 30, 21, -20, 2};	
	
	unsigned int check1 = checksum((void*) vals);
	unsigned int check2 = checksum((void*) vals);
	
	printf("Testing Checksum\n");
	printf("Value 1: %08x\nValue 2: %08x\nValue 3: %08x\nValue 4: %08x\nValue 5: %08x\nChecksum 1: %08x\nChecksum 2: %08x\n",\
		   (unsigned int)vals[0], (unsigned int)vals[1], (unsigned int)vals[2], (unsigned int)vals[3],\
		   (unsigned int)vals[4], (unsigned int)check1, (unsigned int)check2);
	printf("Checksum expected: ffffffd9\n");
	
	return 0;
}
