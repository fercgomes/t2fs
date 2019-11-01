
/**
*/
#include "t2fs.h"

/*------------------ FUNÇÕES FORA DA API -----------------*/

unsigned int checksum(BYTE values[20]);
int load_MBR(MBR* mbr);
int load_superblock(int partition, SUPERBLOCK* spb);
int is_superblock(SUPERBLOCK spb);

/*------------------ FORWARD DECLARATIONS --------*/
void print_superblock(SUPERBLOCK spb);  // Defined at utils.h

/*--------------------- FUNÇÕES DA API -------------------*/

/*-----------------------------------------------------------------------------
Função:	Informa a identificação dos desenvolvedores do T2FS.
-----------------------------------------------------------------------------*/
int identify2 (char *name, int size) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Formata logicamente uma partição do disco virtual t2fs_disk.dat para o sistema de
		arquivos T2FS definido usando blocos de dados de tamanho 
		corresponde a um múltiplo de setores dados por sectors_per_block.
-----------------------------------------------------------------------------*/
int format2(int partition, int sectors_per_block) {
	if (partition < 0 || partition >= 4){
		printf("Error at format2: invalid partition number.\n\tExpects a value between 0 and 3, inclusive.\n\tReceived: %d.\n", partition);
		return -1;
	}
	if (sectors_per_block <= 0) {
		printf("Error at format2: invalid sector_per_block value.\n\tExpects a value above 0.\n\tReceived %d.\n", sectors_per_block);
		return -1;
	}
	
	MBR mbr;
	if (load_MBR(&mbr)){
		printf("Error at format2: couldn't load the MBR\n");
		return -1;
	}

	unsigned int part_sectors;
	unsigned int first_sector;
	unsigned int last_sector;
	unsigned int diskSize;
	unsigned int inodeAreaSize;
	unsigned int freeInodeBitmapSize;
	unsigned int freeBlocksBitmapSize;
	unsigned int blockSizeBytes = SECTOR_SIZE * sectors_per_block;
	unsigned int inodesPerBlock = blockSizeBytes/sizeof(INODE2);
	unsigned int maxInodesPart;
	switch (partition){
		case 0:
			last_sector = mbr.partitionZeroLastSectorAddr;
			first_sector = mbr.partitionZeroFirstSectorAddr;
			break;
		case 1:
			last_sector = mbr.partitionOneLastSectorAddr;
			first_sector = mbr.partitionOneFirstSectorAddr;
			break;
		case 2:
			last_sector = mbr.partitionTwoLastSectorAddr;
			first_sector = mbr.partitionTwoFirstSectorAddr;
			break;
		case 3:
			last_sector = mbr.partitionThreeLastSectorAddr;
			first_sector = mbr.partitionThreeFirstSectorAddr;
			break;
	}
	part_sectors = last_sector - first_sector;
	
	diskSize = part_sectors/sectors_per_block;
	
	// Calculating the ceil without the math lib (this avoids link dependencies)
	float inodearea_f = 0.1*diskSize;
	inodeAreaSize = (unsigned int) inodearea_f;
	inodearea_f = inodearea_f - inodeAreaSize;
	if (inodearea_f >= 0.001) inodeAreaSize += 1;

	maxInodesPart = inodeAreaSize * inodesPerBlock;
	freeInodeBitmapSize = maxInodesPart/(8*blockSizeBytes)+1;
	freeBlocksBitmapSize = diskSize/(8*blockSizeBytes)+1;
	
	unsigned int metadata_blocks = 1 + inodeAreaSize + freeInodeBitmapSize + freeBlocksBitmapSize;
	if (metadata_blocks >= diskSize) {
		printf("Error at format2: the blocks used for metadata are occupying the whole disk. Aborting.\n");
		return -1;
	}

	/*
	printf("secpart %d\nbsb %d\nfblocksbm %d\nfinodesbm %d\ninodearea %d\nmaxinodes %d\ndisksize %d\n\n",\
		   part_sectors, blockSizeBytes, freeBlocksBitmapSize, freeInodeBitmapSize, inodeAreaSize, maxInodesPart, diskSize);
	*/
	
	SUPERBLOCK spb;
	spb.id[0] = 'T';
	spb.id[1] = '2';
	spb.id[2] = 'F';
	spb.id[3] = 'S';
	spb.version = 0x7e32;
	spb.superblockSize = 1;
	
	spb.freeBlocksBitmapSize = freeBlocksBitmapSize;
	spb.freeInodeBitmapSize = freeInodeBitmapSize;
	spb.inodeAreaSize = inodeAreaSize;
	spb.blockSize = sectors_per_block;
	spb.diskSize = diskSize;
	
	BYTE checksum_vals[20];
	memcpy((void*)checksum_vals, (void*)&spb, 20);
	spb.Checksum = checksum(checksum_vals);
	
	printf("Creating Superblock at partition %d\n", partition);
	print_superblock(spb);

	unsigned char buffer[SECTOR_SIZE] = {0};
	
	// Filling the partition with zeros
	int i;
	for (i = first_sector; i < last_sector; i++) {
		if(write_sector(i, buffer)) {
			printf("Error at format2: couldn't write sector while cleaning the partition. Aborting.\n");
			return -1;
		}
	}
	
	memcpy((void*)buffer, (void*)&spb, sizeof(spb));
	if(write_sector(first_sector, buffer)) {
		printf("Error at format2: couldn't write superblock to the first sector of the partition. Aborting.\n");
		return -1;
	}
	
	return 0;
}

/*-----------------------------------------------------------------------------
Função:	Monta a partição indicada por "partition" no diretório raiz
-----------------------------------------------------------------------------*/
int mount(int partition) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Desmonta a partição atualmente montada, liberando o ponto de montagem.
-----------------------------------------------------------------------------*/
int unmount(void) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um novo arquivo no disco e abrí-lo,
		sendo, nesse último aspecto, equivalente a função open2.
		No entanto, diferentemente da open2, se filename referenciar um 
		arquivo já existente, o mesmo terá seu conteúdo removido e 
		assumirá um tamanho de zero bytes.
-----------------------------------------------------------------------------*/
FILE2 create2 (char *filename) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para remover (apagar) um arquivo do disco. 
-----------------------------------------------------------------------------*/
int delete2 (char *filename) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um arquivo existente no disco.
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um arquivo.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para realizar a leitura de uma certa quantidade
		de bytes (size) de um arquivo.
-----------------------------------------------------------------------------*/
int read2 (FILE2 handle, char *buffer, int size) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para realizar a escrita de uma certa quantidade
		de bytes (size) de  um arquivo.
-----------------------------------------------------------------------------*/
int write2 (FILE2 handle, char *buffer, int size) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um diretório existente no disco.
-----------------------------------------------------------------------------*/
int opendir2 (void) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para ler as entradas de um diretório.
-----------------------------------------------------------------------------*/
int readdir2 (DIRENT2 *dentry) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um diretório.
-----------------------------------------------------------------------------*/
int closedir2 (void) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um caminho alternativo (softlink)
-----------------------------------------------------------------------------*/
int sln2 (char *linkname, char *filename) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um caminho alternativo (hardlink)
-----------------------------------------------------------------------------*/
int hln2(char *linkname, char *filename) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função: Carrega o Master Boot Record 
-----------------------------------------------------------------------------*/
int load_MBR(MBR* mbr) {
	if (mbr == NULL) return -1;
	BYTE buffer[SECTOR_SIZE];
	
	int error = read_sector(0, buffer);
	
	if (error) return -1;
	
	memcpy((void*)mbr, (void*)buffer, SECTOR_SIZE);
	
	return 0;
}

/*-----------------------------------------------------------------------------
Função: retorna o checksum de 20 bytes.
-----------------------------------------------------------------------------*/
unsigned int checksum(BYTE values[20]) {
	unsigned int val_as_int[5];
	memcpy((void*)val_as_int, (void*)values, 20);
	unsigned int checksum = 0;
	int i;
	for (i = 0; i < 5; i++){
		checksum += val_as_int[i];
	}
	
	checksum = ~checksum;
	return checksum;
}

int load_superblock(int partition, SUPERBLOCK* spb) {
	if (spb == NULL) return -1;
	if (partition < 0 || partition > 3) return -1;
		
	BYTE buffer[SECTOR_SIZE];
	MBR mbr;
	
	if (load_MBR(&mbr)) return -1;
	
	unsigned int first_sector;
	switch(partition) {
		case 0:
			first_sector = mbr.partitionZeroFirstSectorAddr;
			break;
		case 1:
			first_sector = mbr.partitionOneFirstSectorAddr;
			break;
		case 2:
			first_sector = mbr.partitionTwoFirstSectorAddr;
			break;
		case 3:
			first_sector = mbr.partitionThreeFirstSectorAddr;
			break;
	}
	
	if(read_sector(first_sector, buffer)) return -1;
		
	memcpy((void*)spb, buffer, sizeof(SUPERBLOCK));
	
	return 0;
}

int is_superblock(SUPERBLOCK spb) {
	unsigned char values[20];
	unsigned int checksum_v;
	memcpy((void*)values, (void*)&spb, 20);
	checksum_v = checksum(values);
}

int write_block(unsigned int blockId) {
	//partition must be mounted, since we need the initial block position of the partition
	return -1;
}
