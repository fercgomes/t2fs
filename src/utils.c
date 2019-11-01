
#include "utils.h"

void print_MBR(MBR mbr) {
	printf("--- Imprimindo MBR ---\n");
	printf("Versao do disco/trabalho: 0x%x\n", mbr.diskVersion);
	printf("Tamanho do setor em bytes: 0x%x\n", mbr.sectorSize);
	printf("Byte inicial da tabela de particoes: 0x%x\n", mbr.positionPartitionZero);
	printf("Quantidade de particoes no disco: 0x%x\n", mbr.totalNumberPartitions);
	printf("Endereco do primeiro setor da particao #0: 0x%x\n", mbr.partitionZeroFirstSectorAddr);
	printf("Endereco do ultimo setor da particao #0: 0x%x\n", mbr.partitionZeroLastSectorAddr);
	printf("Nome da particao #0: %s\n", mbr.partitionZeroName);
	printf("Endereco do primeiro setor da particao #1: 0x%x\n", mbr.partitionOneFirstSectorAddr);
	printf("Endereco do ultimo setor da particao #1: 0x%x\n", mbr.partitionOneLastSectorAddr);
	printf("Nome da particao #1: %s\n", mbr.partitionOneName);
	printf("Endereco do primeiro setor da particao #2: 0x%x\n", mbr.partitionTwoFirstSectorAddr);
	printf("Endereco do ultimo setor da particao #2: 0x%x\n", mbr.partitionTwoLastSectorAddr);
	printf("Nome da particao #2: %s\n", mbr.partitionTwoName);
	printf("Endereco do primeiro setor da particao #3: 0x%x\n", mbr.partitionThreeFirstSectorAddr);
	printf("Endereco do ultimo setor da particao #3: 0x%x\n", mbr.partitionThreeLastSectorAddr);
	printf("Nome da particao #3: %s\n", mbr.partitionThreeName);
	printf("----------------------\n");
}

void print_superblock(SUPERBLOCK spb) {
	printf("--- Imprimindo Superbloco ---\n");
	printf("id: %c%c%c%c\n", spb.id[0], spb.id[1], spb.id[2], spb.id[3]);
	printf("version: %x\n", spb.version);
	printf("superblockSize: %d\n", spb.superblockSize);
	printf("freeBlocksBitmapSize: %d\n", spb.freeBlocksBitmapSize);
	printf("freeInodeBitmapSize: %d\n", spb.freeInodeBitmapSize);
	printf("inodeAreaSize: %d\n", spb.inodeAreaSize);
	printf("blockSize: %d\n", spb.blockSize);
	printf("diskSize: %d\n", spb.diskSize);
	printf("Checksum: %08x\n", spb.Checksum);
	printf("----------------------\n");

}
