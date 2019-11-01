
#include <stdio.h>
#include <t2fs.h>
#include <utils.h>

typedef struct sFila2 FILA2;
typedef struct s_swofl_entry SWOFL_ENTRY;
typedef struct s_pwofl_entry PWOFL_ENTRY;
typedef struct t2fs_record DIRENT2;
int swofl_init();
int swofl_destroy();
int pwofl_init();
int pwofl_destroy();
int create_swofl_entry(SWOFL_ENTRY* swofl_entry, DIRENT2* dir_entry);
int delete_swofl_entry(SWOFL_ENTRY* entry);
int create_pwofl_entry(PWOFL_ENTRY* pwofl_entry, SWOFL_ENTRY* swofl_entry);
int delete_pwofl_entry(PWOFL_ENTRY* entry);
void print_swofl_list(FILA2 SWOFL);
void print_pwofl_list(FILA2 PWOFL);
extern FILA2* SWOFL;
extern FILA2* PWOFL;

int main() {
	printf("SWOFL creation1: %s\n", swofl_init() ? "NOT OK" : "OK");
	printf("SWOFL destroy1: %s\n", swofl_destroy() ? "NOT OK" : "OK");
	printf("PWOFL creation1: %s\n", pwofl_init() ? "NOT OK" : "OK");
	printf("PWOFL destroy1: %s\n", pwofl_destroy() ? "NOT OK" : "OK");
	
	printf("SWOFL creation2: %s\n", swofl_init() ? "NOT OK" : "OK");
	printf("PWOFL creation2: %s\n", pwofl_init() ? "NOT OK" : "OK");
	
	DIRENT2* fakedir[5];
	SWOFL_ENTRY* swofl_entry[5];
	PWOFL_ENTRY* pwofl_entry[5];
	
	int nodetobedeleted = 3;
	
	int i;
	for (i = 0; i < 5; i++) {
		fakedir[i] = malloc(sizeof(DIRENT2));
		swofl_entry[i] = malloc(sizeof(SWOFL_ENTRY));
		pwofl_entry[i] = malloc(sizeof(PWOFL_ENTRY));
		printf("\tCreating swofl entry %d: %s\n", i, create_swofl_entry(swofl_entry[i], fakedir[i]) ? "NOT OK" : "OK");
		printf("\tCreating pwofl entry %d: %s\n", i, create_pwofl_entry(pwofl_entry[i], swofl_entry[i]) ? "NOT OK" : "OK");
	}
	
	print_swofl_list(*SWOFL);
	print_pwofl_list(*PWOFL);
	
	printf("\n====== Deleting entries at %d: %s\n\n", nodetobedeleted, delete_pwofl_entry(pwofl_entry[nodetobedeleted]) ? "NOT OK" : "OK");

	print_swofl_list(*SWOFL);
	print_pwofl_list(*PWOFL);
	
	for (i = 0; i < 5; i++) {
		if (i == nodetobedeleted)
			continue;
		printf("\tDeleting pwofl entry %d: %s\n", i, delete_pwofl_entry(pwofl_entry[i]) ? "NOT OK" : "OK");
	}
	
	printf("\nfailures expected:\n");
	print_swofl_list(*SWOFL);
	print_pwofl_list(*PWOFL);
	printf("\n");
	
	for (i = 0; i < 5; i++) {	
		printf("SWOFL deleted: %s\n", swofl_entry[i] ? "OK" : "NOT OK");
		printf("PWOFL deleted: %s\n", pwofl_entry[i] ? "OK" : "NOT OK");
	}
	
	printf("PWOFL destroy2: %s\n", pwofl_destroy() ? "NOT OK" : "OK");
	printf("SWOFL destroy2: %s\n", swofl_destroy() ? "NOT OK" : "OK");
	
	return 0;
}
