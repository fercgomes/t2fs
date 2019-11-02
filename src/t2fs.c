
/**
*/
#include "t2fs.h"

const int _inodes_per_sector = SECTOR_SIZE/sizeof(INODE2);

/*---------------- ESTRUTURAS USADAS NO SISTEMA -----------*/

typedef struct s_partition {
	unsigned int first_inodeblock;
	unsigned int first_inode_sector;
	unsigned int first_block;
	unsigned int first_block_sector;
	unsigned int max_inode_id;
	unsigned int max_block_id;
} PARTITION;

typedef struct s_thedir THEDIR;

typedef struct s_swofl_entry SWOFL_ENTRY;

static unsigned int pwofl_id = 0;
typedef struct s_pwofl_entry PWOFL_ENTRY;

unsigned int spb_sector = 0;
SUPERBLOCK* spb = NULL;	// Open Superblock
PARTITION part;
THEDIR* thedir = NULL;  	// Open directory
FILA2* SWOFL = NULL;  	// System Wide Open File List
FILA2* PWOFL = NULL;		// Process Wide Open File List

/*------------------ FUNÇÕES FORA DA API -----------------*/

unsigned int checksum(BYTE values[20]);
int load_MBR(MBR* mbr); // Retorna zero se falso, outro número se verdadeiro.
int load_superblock(int partition, SUPERBLOCK* spb); // Retorna o número do setor do superbloco, zero se falha.
int is_superblock(SUPERBLOCK spb); // Retorna zero se falso, outro número se verdadeiro.
int is_mounted(); // Retorna zero se falso, outro número se verdadeiro.
int is_dir_open(); // Retorna zero se falso, outro número se verdadeiro.

int swofl_init(); // Inicializa a system wide open file list - Usada na opendir
int swofl_destroy(); // Destroi a SWOFL - Usada na closedir !!!NAO GARANTE QUE OS ARQUIVOS FORAM SALVOS. TODA ESCRITA DEVE SER REALIZADA POR CREATE/WRITE, NAO AQUI
int pwofl_init(); // Inicializa a process wide open file list - Usada na opendir
int pwofl_destroy(); // Destroi a PWOFL - Usada na closedir !!!NAO GARANTE QUE OS ARQUIVOS FORAM SALVOS. TODA ESCRITA DEVE SER REALIZADA POR CREATE/WRITE, NAO AQUI
int create_swofl_entry(SWOFL_ENTRY* swofl_entry, DIRENT2* dir_entry); // Cria uma entrada para a SWOFL - e adiciona na lista (NAO USAR MANUALMENTE)
int delete_swofl_entry(SWOFL_ENTRY* entry); // Remove uma entrada da SWOFL - e remove da lista (NAO USAR MANUALMENTE)
int remove_node_swofl(NODE2* node); // Remove um nodo da lista SWOFL (NAO USAR MANUALMENTE)
int create_pwofl_entry(PWOFL_ENTRY* pwofl_entry, SWOFL_ENTRY* swofl_entry); // Cria uma entrada para a PWOFL - Usada para abrir arquivo
int delete_pwofl_entry(PWOFL_ENTRY* entry); // Remove uma entrada da PWOFL - e remove da lista - Usada para fechar arquivo
int remove_node_pwofl(NODE2* node); // Remove um nodo da lista PWOFL (NAO USAR MANUALMENTE)
unsigned int generate_file_id(); // Gera um id para arquivo aberto (que a princípio nem vai ser usado, pelo jeito)

int filename(BYTE filename_out[51], BYTE* filename_in); // Verifica se o filename_in está ok e, se sim, copia para filename_out e retorna zero. Retorna -1 se inválido
int find_open_file(BYTE filename[51], SWOFL_ENTRY** capture); // Encontra um arquivo aberto com o dado nome. Espera um nome válido - Usada para abrir arquivo e testar se arquivo está aberto

int localize_freeinode(); // Retorna negativo se erro, zero se não achou ou o ID do bloco (positivo) (NAO USAR)
int allocate_inode(int id); // Retorna zero se sucesso, outro número se fracasso (NAO USAR)
int deallocate_inode(int id); // Retorna zero se sucesso, outro número se fracasso - Usada na delete_inode
unsigned int inodeid_to_sector(int id); // Retorna o número do setor (RELATIVO A A PARTIÇÃO) do inode (NAO USAR)
unsigned int inodeid_in_sector(int id); // Retorna o a posição do inode internamente ao setor (em ID, não em bytes) (NAO USAR)
int write_new_inode(DIRENT2* dentry); // Cria um inodo vazio e adiciona ao dentry. Retorna 0 se sucesso e outro número se falha. - Usada na criação de arquivo inexistente.
int write_inode(DIRENT2 dentry, INODE2 inode); // Escreve inode no disco. Retorna 0 se sucesso e outro número se falha. - Usada na atualização de arquivo (OU "CRIAR" ARQUIVO EXISTENTE)
int load_inode(DIRENT2 dentry, INODE2* inode); // Carrega um inode do disco. Retorna 0 se sucesso e outro número se falha. - Usada na função de ler/escrever arquivo
int delete_inode(INODE2* inode); // NOT IMPLEMENTED - Usada na função de deletar arquivo

int localize_freeblock(); // Retorna negativo se erro, zero se não achou ou o ID do bloco (positivo)
int allocate_freeblock(int id); // Retorna negativo se erro, zero se não achou ou o ID do bloco (positivo)

/*------------------ FORWARD DECLARATIONS --------*/
void print_superblock(SUPERBLOCK spb);  // Defined at utils.h


/*--------------------- FUNÇÕES DA API ----------------*/

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
	
	SUPERBLOCK spb_;
	spb_.id[0] = 'T';
	spb_.id[1] = '2';
	spb_.id[2] = 'F';
	spb_.id[3] = 'S';
	spb_.version = 0x7e32;
	spb_.superblockSize = 1;
	
	spb_.freeBlocksBitmapSize = freeBlocksBitmapSize;
	spb_.freeInodeBitmapSize = freeInodeBitmapSize;
	spb_.inodeAreaSize = inodeAreaSize;
	spb_.blockSize = sectors_per_block;
	spb_.diskSize = diskSize;
	
	BYTE checksum_vals[20];
	memcpy((void*)checksum_vals, (void*)&spb_, 20);
	spb_.Checksum = checksum(checksum_vals);
	
	printf("Creating Superblock at partition %d\n", partition);
	print_superblock(spb_);

	unsigned char buffer[SECTOR_SIZE] = {0};
	
	// Filling the partition with zeros
	int i;
	for (i = first_sector; i < last_sector; i++) {
		if(write_sector(i, buffer)) {
			printf("Error at format2: couldn't write sector while cleaning the partition. Aborting.\n");
			return -1;
		}
	}
	
	memcpy((void*)buffer, (void*)&spb_, sizeof(SUPERBLOCK));
	if(write_sector(first_sector, buffer)) {
		printf("Error at format2: couldn't write superblock to the first sector of the partition. Aborting.\n");
		return -1;
	}
	
	if (openBitmap2(first_sector)) {
		printf("Error at format2: couldn't load the bitmaps, but the superblock is already at the disk.\nThe system consistency is at stake. It is recommended to format it again.\n");
		return -1;
	}
	for (i = 0; i < metadata_blocks; i++) {
		if (setBitmap2 (BM_BLOCK, i, 1)) {
			printf("Error at format2: couldn't set a bit at the bitmap.\nThe system's consistency is at stake. It is recommended to format it again.\n");
			return -1;
		}
	}
	
	if(setBitmap2(BM_INODE, 0, 1)) {
		printf("Error at format2: couldn't mark the directory bit as used.\nThe system consistency is at stake; It is recommended to format it again.\n");
		return -1;
	}
	
	INODE2 dirInode;
	dirInode.blocksFileSize = 0;
	dirInode.bytesFileSize = 0;
	dirInode.dataPtr[0] = -1;
	dirInode.dataPtr[1] = -1;
	dirInode.singleIndPtr = -1;
	dirInode.doubleIndPtr = -1;
	dirInode.RefCounter = 1;
	
	unsigned int firstInodeBlock = freeBlocksBitmapSize + freeInodeBitmapSize;
	
	memcpy((void*)buffer, (void*)&dirInode, sizeof(INODE2));
	if(write_sector(firstInodeBlock, buffer)) {
		printf("Error at format2: couldn't write the directory inode to disk.\nThe system consistency is at stake; It is recommended to format it again.\n");
		return -1;
	}
	
	if (closeBitmap2()) {
		printf("Error at format2: the bitmaps could not be closed successfuly.\nThe system consistency is at stake. It is recommended to format it again.\n");
		return -1;
	}
	
	return 0;
}

/*-----------------------------------------------------------------------------
Função:	Monta a partição indicada por "partition" no diretório raiz
-----------------------------------------------------------------------------*/
int mount(int partition) {
	if(is_mounted()) {
		printf("A partition is already mounted. Unmount it to proceed.\n");
		return -1;
	}
	
	spb = (SUPERBLOCK*) malloc(sizeof(SUPERBLOCK));
	spb_sector = load_superblock(partition, spb);
	if (!spb_sector) {
		printf("Couldn't load the superblock of the given partition\nBe sure that you provided a partition id belonging to 1, 2, 3 or 4.\n");
		free(spb);
		spb = NULL;
		spb_sector = 0;
		return -1;
	}
	
	if (!is_superblock(*spb)) {
		printf("The given partition has not been formatted yet.\nFormat it to use it.\n");
		free(spb);
		spb = NULL;
		spb_sector = 0;
		return -1;
	}
	
	part.first_inodeblock = spb->superblockSize + spb->freeBlocksBitmapSize + spb->freeInodeBitmapSize;
	part.first_inode_sector = spb_sector + part.first_inodeblock*spb->blockSize;
	part.first_block = part.first_inodeblock + spb->inodeAreaSize;
	part.first_block_sector = spb_sector + part.first_block*spb->blockSize;
	part.max_inode_id = spb->inodeAreaSize * spb->blockSize * SECTOR_SIZE / sizeof(INODE2) - 1;
	part.max_block_id = spb->diskSize - part.first_block + 1;
	return 0;
}

/*-----------------------------------------------------------------------------
Função:	Desmonta a partição atualmente montada, liberando o ponto de montagem.
-----------------------------------------------------------------------------*/
int umount(void) {
	if (!is_mounted()) {
		printf("No partition to unmount.\n");
		return -1;
	}
	
	if (is_dir_open()) {
		printf("Close the directory before unmounting.\n");
		return -1;
	}
	
	free(spb);
	spb = NULL;
	spb_sector = 0;
	return 0;
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
	if (!is_mounted()) {
		printf("Must have a partition mounted before opening a directory.\n");
		return -1;
	}
	if (is_dir_open()) {
		printf("Must close the current directory before opening it again\n");
		return -1;
	}
	
	// MUST BE FINISHED
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

/*-----------------------------------------------------------------------------
Função: carrega o superbloco da partição
-----------------------------------------------------------------------------*/
int load_superblock(int partition, SUPERBLOCK* spb) {
	if (spb == NULL) return 0;
	if (partition < 0 || partition > 3) return 0;
		
	BYTE buffer[SECTOR_SIZE];
	MBR mbr;
	
	if (load_MBR(&mbr)) return 0;
	
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
	
	if(read_sector(first_sector, buffer)) return 0;
		
	memcpy((void*)spb, buffer, sizeof(SUPERBLOCK));
	
	return first_sector;
}

/*-----------------------------------------------------------------------------
Função: testa se o dado superbloco é válido
-----------------------------------------------------------------------------*/
int is_superblock(SUPERBLOCK spb) {
	unsigned char values[20];
	unsigned int checksum_v;
	memcpy((void*)values, (void*)&spb, 20);
	checksum_v = checksum(values);
	
	return spb.Checksum == checksum_v;
}

int is_mounted() {
	return (int) spb;
}

int is_dir_open() {
	return is_mounted() && thedir;
}

int write_block(unsigned int blockId) {
	//partition must be mounted, since we need the initial block position of the partition
	return -1;
}

int swofl_init() {
	if (SWOFL)
		return -1;
	
	SWOFL = malloc(sizeof(FILA2));
	if (CreateFila2(SWOFL)) {
		SWOFL = NULL;
		return -1;
	}
	return 0;
}

int swofl_destroy() {
	if (!SWOFL)
		return -1;
	
	if (SWOFL->first) // Not empty
		if (FirstFila2(SWOFL)) // Couldn't put at the first position
			return -1;
			
	SWOFL_ENTRY* entry = NULL;
	while (!FirstFila2(SWOFL)) {
		entry = (SWOFL_ENTRY*) GetAtIteratorFila2(PWOFL);
		if(delete_swofl_entry(entry))
			return -1;
	}
	
	free(SWOFL);
	SWOFL = NULL;
	return 0;
}

int pwofl_init() {
	if (PWOFL)
		return -1;
	
	PWOFL = malloc(sizeof(FILA2));
	if (CreateFila2(PWOFL)) {
		PWOFL = NULL;
		return -1;
	}
	return 0;
}

int pwofl_destroy() {
	if (!PWOFL)
		return -1;
	
	if (PWOFL->first) // Not empty
		if (FirstFila2(PWOFL)) // Couldn't put at the first position
			return -1;
		
	PWOFL_ENTRY* entry = NULL;
	while (!FirstFila2(PWOFL)) {
		entry = (PWOFL_ENTRY*) GetAtIteratorFila2(PWOFL);
		if(delete_pwofl_entry(entry))
			return -1;
	}
	
	free(PWOFL);
	PWOFL = NULL;
	return 0;
}

int create_swofl_entry(SWOFL_ENTRY* swofl_entry, DIRENT2* dir_entry) {
	if (!is_dir_open() || !dir_entry || !swofl_entry)
		return -1;
	
	swofl_entry->dir_entry = dir_entry;
	swofl_entry->refs = 0;
	
	if(AppendFila2(SWOFL, (void*)swofl_entry)) {
		return -1;
	}
	swofl_entry->swofl_container = SWOFL->last;
	return 0;
}

int delete_swofl_entry(SWOFL_ENTRY* entry) {
	if (!entry || !is_dir_open())
		return -1;
	
	if (entry->refs > 0)
		return -1;
	
	free(entry->dir_entry);
	if (remove_node_swofl(entry->swofl_container))
		return -1;
		
	free(entry);
	entry = NULL;
	return 0;
}

int remove_node_swofl(NODE2* node) {
	if (!node)
		return -1;
	
	if (!node->ant) {
		SWOFL->first = node->next;
		if (node->next)
			node->next->ant = NULL;
	} else if (!node->next) {
		node->ant->next = NULL;
		SWOFL->last = node->ant;
	} else {
		node->ant->next = node->next;
		node->next->ant = node->ant;
	}
	
	node->node = NULL;
	free(node);	
	node = NULL;
	SWOFL->it = NULL;
	return 0;
}

int create_pwofl_entry(PWOFL_ENTRY* pwofl_entry, SWOFL_ENTRY* swofl_entry) {
	if (!pwofl_entry || !swofl_entry || !is_dir_open())
		return -1;

	pwofl_entry->id = generate_file_id();
	pwofl_entry->sys_file = swofl_entry;
	pwofl_entry->current_position = 0;
	
	if(AppendFila2(PWOFL, (void*)pwofl_entry)) {
		return -1;
	}
	
	pwofl_entry->pwofl_container = PWOFL->last;
	
	swofl_entry->refs += 1;
	
	return 0;
}

int delete_pwofl_entry(PWOFL_ENTRY* entry) {
	if (!entry || !is_dir_open())
		return -1;
	
	entry->sys_file->refs -= 1;
	if (entry->sys_file->refs == 0)
		if(delete_swofl_entry(entry->sys_file)) {
			return -1;
		}
		
	remove_node_pwofl(entry->pwofl_container);
	free(entry);
	entry = NULL;
	return 0;
}

int remove_node_pwofl(NODE2* node) {
	if (!node)
		return -1;
	if (!node->ant) {
		PWOFL->first = node->next;
		if (node->next)
			node->next->ant = NULL;
	} else if (!node->next) {
		node->ant->next = NULL;
		PWOFL->last = node->ant;
	} else {
		node->ant->next = node->next;
		node->next->ant = node->ant;
	}
	
	node->node = NULL;
	free(node);	
	node = NULL;
	PWOFL->it = NULL;
	return 0;
}

unsigned int generate_file_id() {
	pwofl_id += 1;
	if (pwofl_id == 0)
		printf("Not enough file descriptors. The system may be compromised\nPlease, consider closing the directory and reopening it before proceeding.\n");

	return pwofl_id - 1;
}

int filename(BYTE filename_out[51], BYTE* filename_in) {
	if (!filename_in)
		return -1;
		
	BYTE filename_buffer[51] = {'\0'};
	memcpy((void*)filename_buffer, (void*)filename_in, 50);

	BYTE first_char = 0x21;
	BYTE last_char = 0x7A;
	
	int endstring_found = 0;
	int i;
	for (i = 0; i < 50; i++) {
		if (!endstring_found) {
			if ((filename_buffer[i] < first_char || filename_buffer[i] > last_char) && filename_buffer[i] != '\0') {
				return -1;
			} else if (filename_buffer[i] == '\0') {
				if (i == 0)
					return -1;
				endstring_found = i;
			}
		}
		else {
			filename_buffer[i] = '\0';
		}
	}
	
	if (!endstring_found)
		return -1;
	
	memcpy((void*)filename_out, (void*)filename_buffer, 51);
	
	return 0;
}

int find_open_file(BYTE filename[51], SWOFL_ENTRY** capture) { // Expects an already validated filename
	SWOFL_ENTRY* entry;
	if (!FirstFila2(SWOFL))
		do {
			entry = (SWOFL_ENTRY*) GetAtIteratorFila2(SWOFL);
			if (strcmp((const char*)filename, entry->dir_entry->name) == 0) {
				*capture = entry;
				return 0;
			}
		} while (NextFila2(SWOFL) == 0);
	
	return -1;
}

int localize_freeinode() {
	if (!spb_sector) return -1;
	if(openBitmap2(spb_sector))
		return -1;
	
	int res = searchBitmap2(BM_INODE, 0);
	
	if (closeBitmap2())
		return -1;
	
	return res;
}

int allocate_inode(int id) {
	if (!spb_sector) return -1;
	
	if ( id > part.max_inode_id)
		return -1;
		
	if(openBitmap2(spb_sector))
		return -1;
		
	int res = setBitmap2(BM_INODE, id, 1);
	
	if (closeBitmap2())
		return -1;
	
	return res;
}

int deallocate_inode(int id) {
	if (!spb_sector) return -1;
	
	if ( id > part.max_inode_id)
		return -1;
		
	if(openBitmap2(spb_sector))
		return -1;
		
	int res = setBitmap2(BM_INODE, id, 0);
	
	if (closeBitmap2())
		return -1;
	
	return res;
}

unsigned int inodeid_to_sector(int id) {
	return id / _inodes_per_sector;
}

unsigned int inodeid_in_sector(int id) {
	return id % _inodes_per_sector;
}

int write_new_inode(DIRENT2* dentry) { // Expects a valid dentry struct	
	if (!dentry) return -1;
	dentry->inodeNumber = (unsigned int) -1;
	if (!spb_sector) return -1;
	
	int inodeId = localize_freeinode();
	if (inodeId <= 0)
		return -1;
		
	if (allocate_inode(inodeId))
		return -1;

	unsigned int inode_sector_id = inodeid_to_sector(inodeId) + part.first_inode_sector;
	BYTE buffer[SECTOR_SIZE];
	if (read_sector(inode_sector_id, buffer))
		return -1;

	dentry->inodeNumber = (DWORD) inodeId;
	INODE2 inode;
	inode.blocksFileSize = 0;
	inode.bytesFileSize = 0;
	inode.dataPtr[0] = -1;
	inode.dataPtr[1] = -1;
	inode.singleIndPtr = -1;
	inode.doubleIndPtr = -1;
	inode.RefCounter = 1;
	
	memcpy((void*)&buffer[inodeid_in_sector(inodeId)*sizeof(INODE2)], (void*)&inode, sizeof(INODE2));
	if (write_sector(inode_sector_id, buffer))
		return -1;
		
	return 0;
}

int write_inode(DIRENT2 dentry, INODE2 inode) {
	if (dentry.inodeNumber > part.max_inode_id)
		return -1;
		
	BYTE buffer[SECTOR_SIZE];
	unsigned int inode_sector_id = inodeid_to_sector(dentry.inodeNumber) + part.first_inode_sector;
	if (read_sector(inode_sector_id, buffer))
		return -1;
	
	printf("%d %d %d %d\n", part.max_inode_id, dentry.inodeNumber, inode_sector_id, inodeid_in_sector(dentry.inodeNumber)*sizeof(INODE2));
	memcpy((void*)&buffer[inodeid_in_sector(dentry.inodeNumber)*sizeof(INODE2)], (void*)&inode, sizeof(INODE2));
	if (write_sector(inode_sector_id, buffer))
		return -1;
		
	return 0;
}

int load_inode(DIRENT2 dentry, INODE2* inode) {
	if (!inode) return -1;
	if (dentry.inodeNumber > part.max_inode_id)
		return -1;
	
	if(openBitmap2(spb_sector))
		return -1;
		
	int res = getBitmap2(BM_INODE, dentry.inodeNumber);
	if (res <= 0)
		return -1;
	
	BYTE buffer[SECTOR_SIZE];
	unsigned int inode_sector_id = inodeid_to_sector(dentry.inodeNumber) + part.first_inode_sector;
	if (read_sector(inode_sector_id, buffer))
		return -1;
	
	memcpy((void*)inode, (void*)&buffer[inodeid_in_sector(dentry.inodeNumber)*sizeof(INODE2)], sizeof(INODE2));
	
	if (closeBitmap2())
		return -1;
	
	return 0;
}

int delete_inode(INODE2* inode) { // Carefull to deallocate all blocks involved
	return -1;
}

int localize_freeblock() {
	if (!spb_sector) return -1;
	if(openBitmap2(spb_sector))
		return -1;
	
	int res = searchBitmap2(BM_BLOCK, 0);
	
	if(closeBitmap2())
		return -1;
	
	return res - part.first_block + 1;
}

int allocate_freeblock(int id) {
	if (!spb_sector) return -1;
	
	if ( id > part.max_block_id)
		return -1;
		
	if(openBitmap2(spb_sector))
		return -1;
		
	int res = setBitmap2(BM_BLOCK, id, 1);
	
	if (closeBitmap2())
		return -1;
	
	return res;
}

