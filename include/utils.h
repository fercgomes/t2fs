#ifndef __UTILST2FS___
#define __UTILST2FS___

#include <stdio.h>
#include <string.h>

#include <t2disk.h>
#include <apidisk.h>
#include <t2fs.h>
#include <support.h>

typedef struct s_swofl_entry SWOFL_ENTRY;
typedef struct s_pwofl_entry PWOFL_ENTRY;

void print_MBR(MBR mbr);
void print_superblock(SUPERBLOCK spb);
void print_swofl_entry(SWOFL_ENTRY* entry);
void print_pwofl_entry(PWOFL_ENTRY* entry);
void print_swofl_list(FILA2 SWOFL);
void print_pwofl_list(FILA2 PWOFL);

#endif
