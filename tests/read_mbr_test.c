
#include "t2fs.h"
#include "utils.h"
#include <stdio.h>

int main() {
	MBR mbr;
	
	if(load_MBR(&mbr)) return -1;
	
	print_MBR(mbr);
	
	return 0;
}
