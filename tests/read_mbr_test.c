
#include "t2fs.h"
#include <stdio.h>

int load_MBR(MBR* mbr);
void print_MBR(MBR mbr);

int main() {
	MBR mbr;
	
	if(load_MBR(&mbr)) return -1;
	
	print_MBR(mbr);
	
	return 0;
}
