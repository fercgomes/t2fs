#ifndef __UTILST2FS___
#define __UTILST2FS___

#include <stdio.h>
#include <string.h>
#include "t2disk.h"
#include "apidisk.h"

int load_MBR(MBR* mbr);
void print_MBR(MBR mbr);

#endif
