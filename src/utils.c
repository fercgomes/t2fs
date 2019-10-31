
#include "utils.h"

void print_MBR(MBR mbr) {
	printf("--- Imprimindo MBR ---\n");
	printf("Versao do disco/trabalho: 0x%x\n", mbr.diskVersion);
	printf("Tamanho do setor em bytes: 0x%x\n", mbr.sectorSize);
	printf("Byte inicial da tabela de particoes: 0x%x\n", mbr.positionPartitionZero);
	printf("Quantidade de particoes no disco: 0x%x\n", mbr.totalNumberPartitions);
	printf("Endereco do bloco de setor da particao #0: 0x%x\n", mbr.partitionZeroFirstBlockAddr);
	printf("Endereco do ultimo setor da particao #0: 0x%x\n", mbr.partitionZeroLastBlockAddr);
	printf("Nome da particao #0: %s\n", mbr.partitionZeroName);
	printf("Endereco do bloco de setor da particao #1: 0x%x\n", mbr.partitionOneFirstBlockAddr);
	printf("Endereco do ultimo setor da particao #1: 0x%x\n", mbr.partitionOneLastBlockAddr);
	printf("Nome da particao #1: %s\n", mbr.partitionOneName);
	printf("Endereco do bloco de setor da particao #2: 0x%x\n", mbr.partitionTwoFirstBlockAddr);
	printf("Endereco do ultimo setor da particao #2: 0x%x\n", mbr.partitionTwoLastBlockAddr);
	printf("Nome da particao #2: %s\n", mbr.partitionTwoName);
	printf("Endereco do bloco de setor da particao #3: 0x%x\n", mbr.partitionThreeFirstBlockAddr);
	printf("Endereco do ultimo setor da particao #3: 0x%x\n", mbr.partitionThreeLastBlockAddr);
	printf("Nome da particao #3: %s\n", mbr.partitionThreeName);
	printf("----------------------\n");
}
