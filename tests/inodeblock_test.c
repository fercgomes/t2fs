
#include <stdio.h>
#include <t2fs.h>
#include <utils.h>

int localize_freeinode(); // Retorna negativo se erro, zero se não achou ou o ID do bloco (positivo) (NAO USAR)
int allocate_inode(int id); // Retorna zero se sucesso, outro número se fracasso (NAO USAR)
int deallocate_inode(int id); // Retorna zero se sucesso, outro número se fracasso (erro ao desalocar inode não alocado) - Usar na delete_inode
unsigned int inodeid_to_sector(int id); // Retorna o número do setor (RELATIVO A A PARTIÇÃO) do inode (NAO USAR)
unsigned int inodeid_in_sector(int id); // Retorna o a posição do inode internamente ao setor (em ID, não em bytes) (NAO USAR)
int write_new_inode(DIRENT2* dentry); // Cria um inodo vazio e adiciona ao dentry. Retorna 0 se sucesso e outro número se falha. - Usada na criação de arquivo inexistente.
int write_inode(DIRENT2 dentry, INODE2 inode); // Escreve inode no disco. Retorna 0 se sucesso e outro número se falha. - Usada na atualização de arquivo (OU "CRIAR" ARQUIVO EXISTENTE)
int load_inode(DIRENT2 dentry, INODE2* inode); // Carrega um inode do disco. Retorna 0 se sucesso e outro número se falha. - Usada na função de ler/escrever arquivo

int localize_freeblock(); // Retorna negativo se erro, zero se não achou ou o ID do bloco (positivo)
int allocate_freeblock(int id); // Retorna negativo se erro, zero se não achou ou o ID do bloco (positivo)
int deallocate_block(int id); // Retorna zero se sucesso, outro número se fracasso (erro ao desalocar bloco não alocado)
unsigned int blockid_to_sector(int id); // Retorna -1 se fracasso, ID do bloco se sucesso.
BLOCKBUFFER new_block_buffer(); // Retorna um buffer de bloco, NULL se falha (USE WISELY)
BLOCKBUFFER free_block_buffer(BLOCKBUFFER buffer); // Retorna NULL se sucesso. Segmentation fault se falha lol (USE WISELY)
int write_new_block(BLOCKBUFFER block, int* block_id); // Retorna zero se sucesso e guarda o id do novo bloco em block_id, outro número se falha.
int write_block(BLOCKBUFFER block, int block_id); // Retorna zero se sucesso, outro número se falha (não escreve bloco não alocado)
int load_block(BLOCKBUFFER block_buffer, int id); // Retorna zero se sucesso e conteúdo no block_buffer, retorna outro número se falha.

int blockid_sector_position(int block_pos); // Retorna posição do bloco dentro do setor (NAO USAR)
int block_single_indir_position(int block_pos); // Retorna posição do bloco no bloco de indireção. -1 se falha (NAO USAR)
int block_double_indir_position(int block_pos); // Retorna posição do bloco no bloco de dupla indireção. -1 se falha (NAO USAR)
int create_filled_block(DWORD* storeId); // Cria um bloco cheio de -1. Retorna zero se sucesso e outro valor se falha (NAO USAR) - Usada na criação de blocos de índice
int write_to_single_indir_block(int block_pos, int indir_block_id, BLOCKBUFFER block); // Escreve em um bloco de indireção simples. Retorna zero se sucesso (NAO USAR)
int write_to_double_indir_block(int block_pos, int indir_block_id, BLOCKBUFFER block); // Escreve em um bloco de indireção dupla. Retorna zero se sucesso (NAO USAR)
int write_inode_block(INODE2* inode, BLOCKBUFFER block, unsigned int block_pos); // Escreve um bloco na posição designada. Retorna zero se sucesso, outro número se falha - Usada para writes
int append_block_to_inode(INODE2* inode, BLOCKBUFFER block); // Adiciona um bloco ao final do inode. Retorna zero se sucesso.
int load_block_in_indir(int indir_block_id, BLOCKBUFFER buffer, int block_pos, unsigned int* block_id); // Carrega um bloco em um bloco simplesmente indirecionado. Retorna zero se sucesso (NAO USAR)
int load_block_in_db_indir(int indir_block_id, BLOCKBUFFER buffer, int block_pos, unsigned int* block_id); // Carrega um bloco em um bloco duplamente indirecionado. Retorna zero se sucesso (NAO USAR)
int load_inode_block(INODE2 inode, BLOCKBUFFER block_buffer, unsigned int block_pos, unsigned int* block_id); // Carrega um dado bloco do inode. Retorna zero se sucesso, outro número se falha. - Usada para reads

int remove_inode_content(INODE2* inode); // Remove todo o conteúdo do i-node, mantendo número de referências. Retorna zero se sucesso, outro número se falha - Usada na create
int new_dentry(DIRENT2* dentry); // Cria um dentry inválido, de nome vazio e de com um i-node vazio. Retorna zero se sucesso, outro número se falha - Usada na create 
int empty_dentry(DIRENT2* dentry); // Esvazia um dentry e seta inodeNumber para maior que o máximo da partição (NAO REMOVE INODE). Retorna zero se sucesso, outro número se falha. (NAO USAR)
int delete_dentry(DIRENT2* dentry); // Remove o conteúdo e i-node de um dentry, invalidando-o. Retorna zero se sucesso, outro número se falha. - Usada na delete

typedef struct s_partition PARTITION;
extern PARTITION part;

int main() {
	if(format2(1, 4)) {
		printf("Error while formatting 1.\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 1.\n");
		return -1;
	}
	
	printf("Block in sector - Block id 10, in sector: %d %s\n", blockid_sector_position(10), blockid_sector_position(10) == 10 ? "OK" : "NOT OK");
	printf("Block in sector - Block id 1500, in sector: %d %s\n", blockid_sector_position(1500), blockid_sector_position(1500) == 28 ? "OK" : "NOT OK");
	printf("Block id in indir - Block id 0, in indir: %d %s\n", block_single_indir_position(0), block_single_indir_position(0) == 0 ? "OK" : "NOT OK");
	printf("Block id in indir - Block id 10, in indir: %d %s\n", block_single_indir_position(10), block_single_indir_position(10) == 10 ? "OK" : "NOT OK");
	printf("Block id in indir - Block id 1500, in indir: %d %s\n", block_single_indir_position(1500), block_single_indir_position(1500) == 220 ? "OK" : "NOT OK");
	printf("Block id in double indir - Block id 10, in indir: %d %s\n", block_double_indir_position(10), block_double_indir_position(10) == 0 ? "OK" : "NOT OK");
	printf("Block id in double indir - Block id 1500, in indir: %d %s\n", block_double_indir_position(1500), block_double_indir_position(1500) == 5 ? "OK" : "NOT OK");
	printf("Block id in double indir - Block id 65536, in indir: %d %s\n", block_double_indir_position(65535), block_double_indir_position(65535) == 255 ? "OK" : "NOT OK");
	printf("Block id in double indir - Block id 65537, in indir: %d %s\n", block_double_indir_position(65537), block_double_indir_position(65537) == -1 ? "OK" : "NOT OK");
	
	DWORD id;
	unsigned int bid;
	int i;
	int correct_data;
	BLOCKBUFFER bbuffer = new_block_buffer();
	if (!bbuffer) printf("Failed allocating block buffer: NOT OK\n");
	else if (create_filled_block(&id)) {
		printf("Failed creating filled block: NOT OK\n");
	} else {
		if (!load_block(bbuffer, id)) {
			correct_data = !0;
			for (i = 0; i < 4*SECTOR_SIZE; i++) {
				printf("%d ", bbuffer[i]);
				correct_data = correct_data && bbuffer[i];
			}
			printf("\nMinus one filled block created: %s\n", correct_data ? "OK" : "NOT OK");
		} else {
			printf("Failed loading block - ID: %d - NOT OK\n", id);
		}
	}
	
	if (!load_block(bbuffer, id)) {
		for (i = 0; i < 4*SECTOR_SIZE; i++) {
			printf("%d ", bbuffer[i]);
		}
		printf("\n");
	}

	BLOCKBUFFER dummyblock[5];
	int j;
	for (j = 0; j < 5; j++) {
		dummyblock[j] = NULL;
		dummyblock[j] = new_block_buffer();
		if (!dummyblock[j]) {
			printf("Error allocating dummy: NOT OK\n");
			exit(-1);	
		}
		for (i = 0; i < 100; i++) {
			dummyblock[j][SECTOR_SIZE - i - 12] = i + 112 - j;
			dummyblock[j][SECTOR_SIZE*2 - i - 12] = i + 113 - j;
			dummyblock[j][SECTOR_SIZE*3 - i - 12] = i + 114 - j;
			dummyblock[j][SECTOR_SIZE*4 - i - 12] = i + 115 - j;
		}
		printf("Writing to single indir - Pos %d Res: %d %s\n", j*255, (j*255 < 256 ?  0 : -1), write_to_single_indir_block(j*255, id, dummyblock[j]) == (j*255 < 256 ?  0 : -1) ? "OK" : "NOT OK");
	}

	DWORD id2;
	BLOCKBUFFER bbuffer2 = new_block_buffer();
	if (!bbuffer2) printf("Failed allocating block buffer: NOT OK\n");
	else if (create_filled_block(&id2)) printf("Failed creating filled block: NOT OK\n");
	
	printf("\n\n");
	
	BLOCKBUFFER dummyblock2[5];
	for (j = 0; j < 5; j++) {
		dummyblock2[j] = NULL;
		dummyblock2[j] = new_block_buffer();
		if (!dummyblock2[j]) {
			printf("Error allocating dummy: NOT OK\n");
			exit(-1);
		}
		printf("Loading from single indir - indir id %d - Pos %d %s\n", id, j*255, load_block_in_indir(id, dummyblock2[j], j*255, &bid) == (j*255 < 256 ?  0 : -1) ? "OK" : "NOT OK");
		correct_data = 1;
		
		if (j*255 < 256) {
			for (i = 0; i < 100; i++) {
				correct_data = correct_data \
				&& dummyblock2[j][SECTOR_SIZE - i - 12] == i + 112 - j \
				&& dummyblock2[j][SECTOR_SIZE*2 - i - 12] == i + 113 - j \
				&& dummyblock2[j][SECTOR_SIZE*3 - i - 12] == i + 114 - j \
				&& dummyblock2[j][SECTOR_SIZE*4 - i - 12] == i + 115 - j;
			}
			printf("Data is correct? j:%d - %s\n", j, correct_data ? "OK" : "NOT OK");
		}
	}
	printf("\n\n");
	
	for (j = 0; j < 5; j++) {
		for (i = 0; i < 100; i++) {
			dummyblock2[j][SECTOR_SIZE - i - 8] = i + 102 - j;
			dummyblock2[j][SECTOR_SIZE*2 - i - 8] = i + 103 - j;
			dummyblock2[j][SECTOR_SIZE*3 - i - 8] = i + 104 - j;
			dummyblock2[j][SECTOR_SIZE*4 - i - 8] = i + 105 - j;
		}
		printf("Writing to double indir - pos %d Res: %d %s\n", (j+1)*112, ((j+1)*112 < 256*4 ? 0 : -1), write_to_double_indir_block((j+1)*112, id2, dummyblock2[j]) == ((j+1)*112 < 256*4 ? 0 : -1) ? "OK" : "NOT OK");
	}
	
	printf("\n\n");
	for (j = 0; j < 5; j++) {
		printf("Loading from double indir - indir id %d - Pos %d %s\n", id2, (j+1)*112, load_block_in_db_indir(id2, dummyblock[j], (j+1)*112, &bid) == ((j+1)*112 < 256*4 ? 0 : -1) ? "OK" : "NOT OK");
		correct_data = 1;
		
		if ((j+1)*112) {
			for (i = 0; i < 100; i++) {
				correct_data = correct_data \
							&& dummyblock[j][SECTOR_SIZE - i - 8] == i + 102 - j \
							&& dummyblock[j][SECTOR_SIZE*2 - i - 8] == i + 103 - j \
							&& dummyblock[j][SECTOR_SIZE*3 - i - 8] == i + 104 - j \
							&& dummyblock[j][SECTOR_SIZE*4 - i - 8] == i + 105 - j;
			}
			printf("Data is correct? j:%d - %s\n", j, correct_data ? "OK" : "NOT OK");
		}
	}
	
	printf("\n\n");
	DIRENT2 dentry;
	INODE2 inode;
	if(write_new_inode(&dentry)) printf("Failed creating inode: NOT OK\n");
	if(load_inode(dentry, &inode)) printf("Failed loading inode: NOT OK\n");
	
	for (j = 0; j < 10; j++) {
		if (j/5 == 0) {
			for (i = 0; i < 100; i++) {
				dummyblock[j][SECTOR_SIZE - i - 12] = i + 154 - j;
				dummyblock[j][SECTOR_SIZE*2 - i - 12] = i + 153 - j;
				dummyblock[j][SECTOR_SIZE*3 - i - 12] = i + 152 - j;
				dummyblock[j][SECTOR_SIZE*4 - i - 12] = i + 151 - j;
			}
			printf("Writing inode block %d: %s\n", j, write_inode_block(&inode, dummyblock[j], j) == 0 ? "OK" : "NOT OK");
			inode.blocksFileSize += 1;
		} else {
			printf("Writing inode block %d: %s\n", j, write_inode_block(&inode, dummyblock[j-5], j) == 0 ? "OK" : "NOT OK");
			inode.blocksFileSize += 1;
		}
	}
	
	printf("\n\n");
	for (i = 0; i < 10; i++) {
		if (i/5 == 0) {
			printf("Writing inode block: %s\n", append_block_to_inode(&inode, dummyblock[i]) == 0 ? "OK" : "NOT OK");
			printf("\tInode size: %d %s\n", inode.blocksFileSize, inode.blocksFileSize == 10+i+1 ? "OK" : "NOT OK");

		} else {
			printf("Writing inode block: %s\n", append_block_to_inode(&inode, dummyblock2[i-5]) == 0 ? "OK" : "NOT OK");
			printf("\tInode size: %d %s\n", inode.blocksFileSize, inode.blocksFileSize == 10+i+1 ? "OK" : "NOT OK");
		}
	}
	
	for (j = 0; j < 10; j++) {
		if (j/5 == 0) {
			printf("Loading block from inode - Pos %d %s\n", j, load_inode_block(inode, dummyblock2[j], j, &bid) == 0 ? "OK" : "NOT OK");
			correct_data = 1;
		
			for (i = 0; i < 100; i++) {
				correct_data = correct_data \
							&& dummyblock2[j][SECTOR_SIZE - i - 12] == i + 154 - j \
							&& dummyblock2[j][SECTOR_SIZE*2 - i - 12] == i + 153 - j \
							&& dummyblock2[j][SECTOR_SIZE*3 - i - 12] == i + 152 - j \
							&& dummyblock2[j][SECTOR_SIZE*4 - i - 12] == i + 151 - j;
			}
			printf("Data is correct? j:%d - %s\n", j, correct_data ? "OK" : "NOT OK");
		} else {
			printf("Loading block from inode - Pos %d %s\n", j, load_inode_block(inode, dummyblock2[j-5], j, &bid) == 0 ? "OK" : "NOT OK");
			correct_data = 1;
	
			for (i = 0; i < 100; i++) {
				correct_data = correct_data \
							&& dummyblock2[j-5][SECTOR_SIZE - i - 12] == i + 154 - j+5 \
							&& dummyblock2[j-5][SECTOR_SIZE*2 - i - 12] == i + 153 - j+5 \
							&& dummyblock2[j-5][SECTOR_SIZE*3 - i - 12] == i + 152 - j+5 \
							&& dummyblock2[j-5][SECTOR_SIZE*4 - i - 12] == i + 151 - j+5;
			}
			printf("Data is correct? j:%d - %s\n", j, correct_data ? "OK" : "NOT OK");
		}
	}
	
	for (i = 0; i < 5; i++) {
		free_block_buffer(dummyblock[i]);
		free_block_buffer(dummyblock2[i]);
	}
	
	free_block_buffer(bbuffer);
	free_block_buffer(bbuffer2);
	if(umount()) {
		printf("Error while unmounting 1.\n");
		return -1;
	}	
	
	printf("\n\n");
	
	if(format2(1, 1)) {
		printf("Error while formatting 2.\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 2.\n");
		return -1;
	}
	
	int size = 676;
	BLOCKBUFFER dummyblocks[size];
	
	for (i = 0; i < size; i++) {
		dummyblocks[i] = new_block_buffer();
		if (! dummyblocks[i]) printf("Failed allocating block buffer: NOT OK\n");
	}
	
	if(write_new_inode(&dentry)) printf("Failed creating inode: NOT OK\n");
	if(load_inode(dentry, &inode)) printf("Failed loading inode: NOT OK\n");
	
	for (j = 0; j < size; j++)
		for (i = 0; i < 100; i++) {
			dummyblocks[j][SECTOR_SIZE - i - 12] = i + 112 - j;
		}
			
	for (i = 0; i < size; i++) {
		if (i != size-1) {
			printf("Appending to inode - %d: %s\n", i, append_block_to_inode(&inode, dummyblocks[i]) == 0 ? "OK" : "NOT OK");
			printf("Inode size: %d %s\n", inode.blocksFileSize, inode.blocksFileSize == i+1 ? "OK" : "NOT OK");
		} else {
			printf("Appending to inode - %d: %s\n", i, append_block_to_inode(&inode, dummyblocks[i]) == -1 ? "OK" : "NOT OK");
			printf("Inode size: %d %s\n", inode.blocksFileSize, inode.blocksFileSize == i ? "OK" : "NOT OK");
		}
	}
	
	BLOCKBUFFER dummyblocks2[size];
	
	for (i = 0; i < size; i++) {
		dummyblocks2[i] = new_block_buffer();
		if (! dummyblocks2[i]) printf("Failed allocating block buffer: NOT OK\n");
	}
	
	
	for (j = 0; j < size; j++) {
		if (j != size-1) {
			printf("Loading inode block %d: %s\n", j, load_inode_block(inode, dummyblocks2[j], j, &bid) == 0? "OK" : "NOT OK");
			correct_data = 1;
			for (i = 0; i < 100; i++) {
				correct_data = correct_data && dummyblocks2[j][SECTOR_SIZE - i - 12] == dummyblocks[j][SECTOR_SIZE - i - 12];
			}
			printf("Checking inode data %d - bid %d: %s\n", j, bid, correct_data ? "OK" : "NOT OK");
		} else {
			printf("Failing to load inode block %d: %s\n", j, load_inode_block(inode, dummyblocks[j], j, &bid) == -1? "OK" : "NOT OK");
		}
	}
	
	printf("Removing inode content: %s\n", remove_inode_content(&inode) == 0 ? "OK" : "NOT OK");
	printf("Checking data:\n\tSizes: Blocks %s - Bytes %s\n\tDirect pointers: %s\n\tSingle indir: %s\n\tDouble indir: %s\n", inode.blocksFileSize == 0 ? "OK" : "NOT OK",\
																								inode.bytesFileSize == 0 ? "OK" : "NOT OK",\
																								inode.dataPtr[0] == -1 && inode.dataPtr[1] == -1 ? "OK" : "NOT OK",\
																								inode.singleIndPtr == -1 ? "OK" : "NOT OK",\
																								inode.doubleIndPtr == -1 ? "OK" : "NOT OK");
	
	if(umount()) {
		printf("Error while unmounting 3.\n");
		return -1;
	}	
	
	printf("\n\n");
	
	if(format2(1, 1)) {
		printf("Error while formatting 3.\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 3.\n");
		return -1;
	}

	DIRENT2 dummyDentry;
	if (empty_dentry(&dentry)) printf("Error: failed to create empty dentry. NOT OK\n");
	
	printf("New dentry test: %s\n", new_dentry(&dentry) == 0 ? "OK" : "NOT OK");
	printf("Loading inode from dentry: %s\n", load_inode(dentry, &inode) == 0 ? "OK" : "NOT OK");
	
	unsigned int inodeId = dentry.inodeNumber;
	dummyDentry.inodeNumber = inodeId;
	
	j = 0;
	while(!append_block_to_inode(&inode, dummyblocks[j]) && j < size-1) j++;
	if (j != size-1) printf("Failed to append to inode: NOT OK\n");
	
	dentry.TypeVal = 0x01;
	printf("Deleting dentry: %s\n", delete_dentry(&dentry) == 0 ? "OK" : "NOT OK");

	printf("Trying to load removed inode: %s\n", load_inode(dummyDentry, &inode) != 0 ? "OK" : "NOT OK");
	printf("Checking dentry data:\n");
	printf("\tTypeVal: %08x %s\n", dentry.TypeVal, dentry.TypeVal == 0x0 ? "OK" : "NOT OK");
	printf("\tFilename: %s %s\n", dentry.name, dentry.name[0] == '\0' ? "OK" : "NOT OK");
	printf("\tinodeNumber: %d %s\n", dentry.inodeNumber, dentry.inodeNumber > part.max_inode_id ? "OK" : "NOT OK");
	
	
	if(umount()) {
		printf("Error while unmounting 3.\n");
		return -1;
	}	
	return 0;
}
