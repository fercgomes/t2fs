
#include <stdio.h>
#include <t2fs.h>

int main() {
	
	printf("Formating partition 1: %s\n", format2(1,20) ? "NOT OK" : "OK");
	printf("Mounting partition 1: %s\n", mount(1) ? "NOT OK" : "OK");
	printf("Unmounting partition 1: %s\n", umount() ? "NOT OK" : "OK");
	
	return 0;
}
