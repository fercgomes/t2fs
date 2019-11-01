

#ifndef __T2DSIK___
#define __T2DSIK___

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
	DWORD partitionZeroFirstSectorAddr;
	DWORD partitionZeroLastSectorAddr;
	char partitionZeroName[24];
	DWORD partitionOneFirstSectorAddr;
	DWORD partitionOneLastSectorAddr;
	char partitionOneName[24];
	DWORD partitionTwoFirstSectorAddr;
	DWORD partitionTwoLastSectorAddr;
	char partitionTwoName[24];
	DWORD partitionThreeFirstSectorAddr;
	DWORD partitionThreeLastSectorAddr;
	char partitionThreeName[24];
	BYTE notUsed[120];
} MBR;

#pragma pack(pop)




#endif

