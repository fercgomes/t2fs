

#ifndef __T2DSIK___
#define __T2DSIK___

#include "t2fs.h"
#include "apidisk.h"

typedef int boolean;
#define false 0
#define true (!false)
typedef unsigned char       BYTE;
typedef unsigned short int  WORD;
typedef unsigned int        DWORD;

#pragma pack(push, 1)

/** Dados do MBR */
typedef struct s_mbr{
	WORD diskVersion;
	WORD sectorSize;
	WORD positionPartitionZero;
	WORD totalNumberPartitions;
	DWORD partitionZeroFirstBlockAddr;
	DWORD partitionZeroLastBlockAddr;
	char partitionZeroName[24];
	DWORD partitionOneFirstBlockAddr;
	DWORD partitionOneLastBlockAddr;
	char partitionOneName[24];
	DWORD partitionTwoFirstBlockAddr;
	DWORD partitionTwoLastBlockAddr;
	char partitionTwoName[24];
	DWORD partitionThreeFirstBlockAddr;
	DWORD partitionThreeLastBlockAddr;
	char partitionThreeName[24];
	BYTE notUsed[120];
} MBR;

#pragma pack(pop)



#endif

