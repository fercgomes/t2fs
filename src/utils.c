
#include "utils.h"

int load_MBR(MBR* mbr) {
	BYTE buffer[SECTOR_SIZE];
	
	int error = read_sector(0, buffer);
	
	if (error) return -1;
	
	memcpy((void*)mbr, (void*)buffer, SECTOR_SIZE);
	
	return 0;
}

void print_MBR(MBR mbr) {
	printf("Imprimindo MBR\n");
	printf("  !!!Atencao: todos os valores estao em hexadecimal\n");
	printf("     Nomes de particoes sao strings\n");
	printf("Versao do disco/trabalho: %x\n", mbr.diskVersion);
	printf("Tamanho do setor em bytes: %x\n", mbr.sectorSize);
	printf("Byte inicial da tabela de particoes: %x\n", mbr.positionPartitionZero);
	printf("Quantidade de particoes no disco: %x\n", mbr.totalNumberPartitions);
	printf("Endereco do bloco de setor da particao #0: %x\n", mbr.partitionZeroFirstBlockAddr);
	printf("Endereco do ultimo setor da particao #0: %x\n", mbr.partitionZeroLastBlockAddr);
	printf("Nome da particao #0: %s\n", mbr.partitionZeroName);
	printf("Endereco do bloco de setor da particao #1: %x\n", mbr.partitionOneFirstBlockAddr);
	printf("Endereco do ultimo setor da particao #1: %x\n", mbr.partitionOneLastBlockAddr);
	printf("Nome da particao #1: %s\n", mbr.partitionOneName);
	printf("Endereco do bloco de setor da particao #2: %x\n", mbr.partitionTwoFirstBlockAddr);
	printf("Endereco do ultimo setor da particao #2: %x\n", mbr.partitionTwoLastBlockAddr);
	printf("Nome da particao #2: %s\n", mbr.partitionTwoName);
	printf("Endereco do bloco de setor da particao #3: %x\n", mbr.partitionThreeFirstBlockAddr);
	printf("Endereco do ultimo setor da particao #3: %x\n", mbr.partitionThreeLastBlockAddr);
	printf("Nome da particao #3: %s\n", mbr.partitionThreeName);
}
