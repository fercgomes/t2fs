
#include <stdio.h>
#include <t2fs.h>

int check_filename(BYTE filename_out[51], BYTE* filename_in);

int main() {
	BYTE dummyname[51];
	BYTE goodname[51] = "ImAGoodName";
	BYTE goodname2[51] = "IamAHELLofaGOODname!!!\"'123\\azAZ@";
	BYTE badname[100] = "LLLLOOOOOONGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG";
	BYTE badname2[51] = "";
	BYTE badname3[51] = "{}";
	
	printf("Simple name: %s %s\n", goodname, check_filename(dummyname, goodname) ? "NOT OK" : "OK");
	printf("Symbols name: %s\n", check_filename(dummyname, goodname2) ? "NOT OK" : "OK");
	printf("Long name 1: %s\n", check_filename(dummyname, badname) ? "OK" : "NOT OK");
	printf("Empty name: %s\n", check_filename(dummyname, badname2) ? "OK" : "NOT OK");
	printf("Invalid char name: %s\n", check_filename(dummyname, badname3) ? "OK" : "NOT OK");
	
	return 0;
}
