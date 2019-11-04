
/**
*/
#include "t2fs.h"

const int _inodes_per_sector = SECTOR_SIZE/sizeof(INODE2);
const int _blockid_per_sector = SECTOR_SIZE/sizeof(DWORD);
const int _dirs_in_sector = SECTOR_SIZE/sizeof(DIRENT2);

/*---------------- ESTRUTURAS USADAS NO SISTEMA -----------*/

typedef struct s_thedir THEDIR;

typedef struct s_swofl_entry SWOFL_ENTRY;

static FILE2 pwofl_id = 0;
typedef struct s_pwofl_entry PWOFL_ENTRY;

typedef struct s_partition PARTITION;

typedef BYTE SECTOR[SECTOR_SIZE];
typedef BYTE* BLOCKBUFFER;

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
int create_swofl_entry(SWOFL_ENTRY* swofl_entry, DIRENT2* dir_entry); // Cria uma entrada para a SWOFL - e adiciona na lista
int delete_swofl_entry(SWOFL_ENTRY* entry); // Remove uma entrada da SWOFL - e remove da lista (NAO USAR MANUALMENTE)
int remove_node_swofl(NODE2* node); // Remove um nodo da lista SWOFL (NAO USAR MANUALMENTE)
int create_pwofl_entry(PWOFL_ENTRY* pwofl_entry, SWOFL_ENTRY* swofl_entry); // Cria uma entrada para a PWOFL - Usada para abrir arquivo
int delete_pwofl_entry(PWOFL_ENTRY* entry); // Remove uma entrada da PWOFL - e remove da lista - Usada para fechar arquivo
int remove_node_pwofl(NODE2* node); // Remove um nodo da lista PWOFL (NAO USAR MANUALMENTE)
FILE2 generate_file_id(); // Gera um id para arquivo aberto (que a princípio nem vai ser usado, pelo jeito)

int check_filename(BYTE filename_out[51], BYTE* filename_in); // Verifica se o filename_in está ok e, se sim, copia para filename_out e retorna zero. Retorna -1 se inválido
int find_open_file(BYTE filename[51], SWOFL_ENTRY** capture); // Encontra um arquivo aberto com o dado nome. Espera um nome válido - Usada para abrir arquivo e testar se arquivo está aberto
int search_pwofl(FILE2 id, PWOFL_ENTRY** capture); // Encontra um arquivo aberto na PWOFL dado o handler. Retorna 0 se sucesso, outro número se válido.

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
BLOCKBUFFER new_block_buffer(); // Retorna um buffer de bloco, NULL se falha (USE WISELY, provavelmente desnecessário usar, dadas as outras funções auxiliares)
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

int search_file_in_dir(char* filename, DIRENT2* dentry, int* dentry_block, int* dentry_pos); // Busca uma dentry com o dado nome e retorna no ponteiro para dentry. Retorna zero se sucesso, outro número se fracasso.
int write_to_invalid_dentry_in_dir(DIRENT2 dentry); // Busca uma dentry inválida no diretório e escreve sobre ela. Retorna zero se sucesso, outro número se fracasso. (NÃO USAR)
int write_dentry_to_dir(DIRENT2 dentry); // Escreve uma dentry no diretório. Retorna zero se sucesso, outro número se fracasso.

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
	freeInodeBitmapSize = maxInodesPart/(8*blockSizeBytes) + 1;
	freeBlocksBitmapSize = diskSize/(8*blockSizeBytes) + 1;
	
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
	
	unsigned int firstInodeBlock = freeBlocksBitmapSize + freeInodeBitmapSize + 1;
	
	memcpy((void*)buffer, (void*)&dirInode, sizeof(INODE2));
	if(write_sector(first_sector + firstInodeBlock*sectors_per_block, buffer)) {
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
	unsigned int first_sector = load_superblock(partition, spb);
	if (!first_sector) {
		printf("Couldn't load the superblock of the given partition\nBe sure that you provided a partition id belonging to 1, 2, 3 or 4.\n");
		free(spb);
		spb = NULL;
		return -1;
	}
	
	if (!is_superblock(*spb)) {
		printf("The given partition has not been formatted yet.\nFormat it to use it.\n");
		free(spb);
		spb = NULL;
		return -1;
	}
	
	part.first_sector = first_sector;
	part.first_inodeblock = spb->superblockSize + spb->freeBlocksBitmapSize + spb->freeInodeBitmapSize;
	part.first_inode_sector = part.first_sector + part.first_inodeblock*spb->blockSize;
	part.first_block = part.first_inodeblock + spb->inodeAreaSize;
	part.first_block_sector = part.first_sector + part.first_block*spb->blockSize;
	part.max_inode_id = spb->inodeAreaSize * spb->blockSize * SECTOR_SIZE / sizeof(INODE2) - 1;
	part.max_block_id = spb->diskSize - part.first_block + 1;
	part.bytes_in_block = spb->blockSize * SECTOR_SIZE;
	part.blockids_in_block = part.bytes_in_block/sizeof(DWORD);
	part.dirs_in_block = part.bytes_in_block/sizeof(DIRENT2); // The correct would be dentry, not dir. Too much work to correct
	part.max_dentries = 2 + part.dirs_in_block + part.dirs_in_block*part.dirs_in_block;
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
	if (!is_mounted()) {
		printf("Must have a partition mounted before operating a file.\n");
		return -1;
	}
	if (!is_dir_open()) {
		printf("Must open the directory before the file creation operation\n");
		return -1;
	}
	
	BYTE _filename[51];
	if(check_filename(_filename, (BYTE*)filename)) {
		printf("The filename provided is invalid\n");
		return -1;
	}
	
	SWOFL_ENTRY* swofl_e;
	
	if (!find_open_file(_filename, &swofl_e)) {
		printf("The file you are trying to create is open\n\tCouldn't create the file\n");
		return -1;
	}
	
	DIRENT2 dentry;
	INODE2 inode;
	int dentry_block;
	int dentry_pos;
	if (!search_file_in_dir((char*)_filename, &dentry, &dentry_block, &dentry_pos)) {
		if (load_inode(dentry, &inode)) {
			printf("Unexpected Error occurred at create2: CLDIND\n\tCouldn't create the file\n");
			return -1;
		}
		
		if (remove_inode_content(&inode)) {
			printf("Unexpected Error occurred at create2: CRMINDCT\n\tCoudln't create the file\n");
			return -1;
		}
		
		return 0;
	}
	
	if (new_dentry(&dentry)) {
		printf("Unexpected Error occurred at create2: CNDTR\n\tCouldn't create the file\n");
		return -1;
	}
	
	memcpy((void*)dentry.name, (void*)_filename, 51);
	dentry.TypeVal = 0x01;
	
	if (write_dentry_to_dir(dentry)) {
		printf("Error creating file. Is the directory full?\n");
		return -1;
	}
	
	return 0;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para remover (apagar) um arquivo do disco. 
-----------------------------------------------------------------------------*/
int delete2 (char *filename) {
	if (!is_mounted()) {
		printf("Must have a partition mounted before operating a file.\n");
		return -1;
	}
	if (!is_dir_open()) {
		printf("Must open the directory before the file deletion operation\n");
		return -1;
	}
	
	BYTE _filename[51];
	if(check_filename(_filename, (BYTE*)filename)) {
		printf("The filename provided is invalid\n");
		return -1;
	}
	
	SWOFL_ENTRY* swofl_e;
	
	if (!find_open_file(_filename, &swofl_e)) {
		printf("The file you are trying to remove is open\n\tCouldn't delete the file\n");
		return -1;
	}
	
	DIRENT2 dentry;
	int dentry_block;
	int dentry_pos;
	if (search_file_in_dir((char*)_filename, &dentry, &dentry_block, &dentry_pos)) {
		printf("Couldn't find the given file\n");
		return -1;
	}
		
	if(delete_dentry(&dentry)) {
		printf("Unexpected Error occurre at delete2: DDLDNT\n\tCouldn't delete the file\n");
		return -1;
	}
	
	BLOCKBUFFER bbuffer = new_block_buffer();
	if (!bbuffer) {
		printf("Unexpected Error occurred at delete2: DBBFMLC\n\tCouldn't delete the file\n");
		return -1;
	}
	
	unsigned int bid;
	if (load_inode_block(thedir->inode, bbuffer, dentry_block, &bid)) {
		printf("Unexpected Error occurred at delete2: DLDIND\n\tCouldn't delete the file\n");
		free_block_buffer(bbuffer);
		return -1;
	}
	
	memcpy((void*)&bbuffer[dentry_pos*sizeof(DIRENT2)], (void*)&dentry, sizeof(DIRENT2));
	
	if (write_inode_block(&(thedir->inode), bbuffer, dentry_block)) {
		printf("Unexpected Error occurred at delete2: DWTBBF\n\tCouldn't delete the file\n");
		free_block_buffer(bbuffer);
		return -1;
	}
	
	free_block_buffer(bbuffer);
	
	return 0;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um arquivo existente no disco.
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename) {
	if (!is_mounted()) {
		printf("Must have a partition mounted before operating a file.\n");
		return -1;
	}
	if (!is_dir_open()) {
		printf("Must open the directory before the open file operation\n");
		return -1;
	}
	
	BYTE _filename[51];
	if(check_filename(_filename, (BYTE*)filename)) {
		printf("The filename provided is invalid\n");
		return -1;
	}
	
	SWOFL_ENTRY* swofl_e = NULL;
	PWOFL_ENTRY* pwofl_e = malloc(sizeof(PWOFL_ENTRY));
	
	if (!pwofl_e) {
		printf("Unexpected Error occurred at open2: OPWOFLMLC\n\tCouldn't open the file\n");
		return -1;
	}
	
	if (!find_open_file(_filename, &swofl_e)) {
		if (create_pwofl_entry(pwofl_e, swofl_e)) {
			printf("Unexpected Error occurred at open2: OCPWOFl\n\tCouldn't open the file\n");
			free(pwofl_e);
			return -1;
		}
		return pwofl_e->id;
	}
	
	int dt_block;
	int dt_pos;	
	DIRENT2* dentry = malloc(sizeof(DIRENT2));
	if (!dentry) {
		printf("Unexpected Error occurred at open2: ODTMLC\n\tCouldn't open the file\n");
		free(pwofl_e);
		return -1;
	}
	
	if (search_file_in_dir((char*)_filename, dentry, &dt_block, &dt_pos)) {
		printf("Couldn't find a file with the given name\n");
		free(dentry);
		free(pwofl_e);
		return -1;
	}

	SWOFL_ENTRY* swofl_e2 = malloc(sizeof(SWOFL_ENTRY));
	
	if (!swofl_e2) {
		printf("Unexpected Error occurred at open2: OMSWOFL\n\tCouldn't open the file\n");
		free(dentry);
		free(pwofl_e);
		return -1;
	}
	
	if (create_swofl_entry(swofl_e2, dentry)) {
		printf("Unexpected Error occurred at open2: OCSWOFL\n\tCouldn't open the file\n");
		free(dentry);
		free(pwofl_e);
		free(swofl_e2);
		return -1;
	}
	
	if (create_pwofl_entry(pwofl_e, swofl_e2)) {
		printf("Unexpected Error occurred at open2: OCPWOFl\n\tCouldn't open the file\n");
		free(dentry);
		free(pwofl_e);
		free(swofl_e2);
		return -1;
	}
	return pwofl_e->id;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um arquivo.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle) {
	if (!is_mounted()) {
		printf("Must have a partition mounted before operating a file.\n");
		return -1;
	}
	if (!is_dir_open()) {
		printf("Must open the directory before the closing a file\n");
		return -1;
	}
	
	PWOFL_ENTRY* entry;
	if(search_pwofl(handle, &entry)) {
		printf("Couldn't find the open file referred by the given handle\n");
		return -1;
	}
	
	if (delete_pwofl_entry(entry)) {
		printf("Unexpected Error occurred at close2: CDPWOFL\n\tCouldn't close the file\n");
		return -1;
	}

	return 0;
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
		printf("Must have a partition mounted before operating a directory.\n");
		return -1;
	}
	if (is_dir_open()) {
		printf("Must close the current directory before the opendir operation\n");
		return -1;
	}
	
	thedir = malloc(sizeof(THEDIR));
	if (!thedir) {
		printf("Unexpected Error at opendir2: ODMALLC\n\tCouldn't load directory\n");
		thedir = NULL;
		return -1;
	}
	
	SECTOR buffer;
	unsigned int inode_sector_id = part.first_inode_sector;
	if (read_sector(inode_sector_id, buffer)) {
		printf("Unexpected Error at opendir2: ODSECREAD\n\tCouldn't load directory\n");
		free(thedir);
		thedir = NULL;
		return -1;
	}
	
	memcpy((void*)&thedir->inode, (void*)buffer, sizeof(INODE2));

	if (swofl_init()) {
		printf("Unexpected Error at opendir2: ODSWOFLINIT\n\tCouldn't load directory\n");
		free(thedir);
		thedir = NULL;
		return -1;
	}
	
	if (pwofl_init()) {
		printf("Unexpected Error at opendir2: ODPWOFLINIT\n\tCouldn't load directory\n");
		free(thedir);
		thedir = NULL;
		return -1;
	} 
	
	thedir->current_entry = 0;
	
	return 0;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para ler as entradas de um diretório.
-----------------------------------------------------------------------------*/
int readdir2 (DIRENT2 *dentry) {
	if (!is_mounted()) {
		printf("Must have a partition mounted before operating a directory.\n");
		return -1;
	}
	if (!is_dir_open()) {
		printf("Must have an open directory before the readdir operation\n");
		return -1;
	}
	
	if (!dentry) {
		printf("Error at readdir2: invalid dentry provided\n\tBe sure to provide a valid dentry structure before a readdir operation\n");
		return -1;
	}
	
	if (thedir->current_entry*sizeof(DIRENT2) >= thedir->inode.bytesFileSize) {
		printf("No more directory entries to be read\n");
		dentry->TypeVal = 0x0;
		dentry->inodeNumber = part.max_inode_id+1;
		return -1;
	}
	
	unsigned int block_pos = thedir->current_entry / part.dirs_in_block;

	BLOCKBUFFER block_buffer = new_block_buffer();
	if (!block_buffer) {
		printf("Unexpected Error at readdir2: RDNBBUFFER\n\tCouldn't read the directory\n");
		dentry->TypeVal = 0x0;	
		dentry->inodeNumber = part.max_inode_id+1;
		return -1;
	}
	
	unsigned int block_id;
	if (load_inode_block(thedir->inode, block_buffer, block_pos, &block_id)) {
		printf("Unexpected Error at readdir2: RDLBBUFFER\n\tCouldn't read the directory\n");
		dentry->TypeVal = 0x0;
		dentry->inodeNumber = part.max_inode_id+1;
		free_block_buffer(block_buffer);
		return -1;
	}
	
	unsigned int pos_in_block = thedir->current_entry % part.dirs_in_block;
	
	memcpy((void*)dentry, (void*)&block_buffer[pos_in_block*sizeof(DIRENT2)], sizeof(DIRENT2));
	
	free_block_buffer(block_buffer);
	
	thedir->current_entry += 1;
		
	return 0;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um diretório.
-----------------------------------------------------------------------------*/
int closedir2 (void) {
	if (!is_mounted()) {
		printf("Must have a partition mounted before operating a directory.\n");
		return -1;
	}
	if (!is_dir_open()) {
		printf("No directory is open to be closed\n");
		return -1;
	}
	
	int error = 0;
	if (!FirstFila2(SWOFL)) {
		printf("Close all files before closing the directory\n");
		return -1;
	}
	
	if (pwofl_destroy()) {
		printf("Unexpected Error at closedir2: CDPWOFLDSTR\n\tOpen files may be lost\n");
		PWOFL = NULL;
		error = -1;
	}
	
	if (swofl_destroy()) {
		printf("Unexpected Error at closedir2: CDSWOFLDSTR\n\tOpen files may be lost\n");
		SWOFL = NULL;
		error = -1;
	}
	
	free(thedir);
	thedir = NULL;
	
	return error;
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

FILE2 generate_file_id() {
	pwofl_id += 1;
	if (pwofl_id == 0)
		printf("Not enough file descriptors. The system may be compromised\nPlease, consider closing the directory and reopening it before proceeding.\n");

	return pwofl_id - 1;
}

int check_filename(BYTE filename_out[51], BYTE* filename_in) {
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

int search_pwofl(FILE2 id, PWOFL_ENTRY** capture) {
	if (!is_dir_open() || !capture || !SWOFL || !PWOFL)
		return -1;
	
	PWOFL_ENTRY* entry;
	if (!FirstFila2(PWOFL))
		do {
			entry = (PWOFL_ENTRY*) GetAtIteratorFila2(PWOFL);
			if (id == entry->id) {
				*capture = entry;
				return 0;
			}
		} while (NextFila2(PWOFL) == 0);
	
	return -1;
}

int localize_freeinode() {
	if (!is_mounted()) return -1;
	if(openBitmap2(part.first_sector))
		return -1;
	
	int res = searchBitmap2(BM_INODE, 0);
	
	if (closeBitmap2())
		return -1;
	
	return res;
}

int allocate_inode(int id) {
	if (!is_mounted()) return -1;
	
	if ( id > part.max_inode_id)
		return -1;
		
	if(openBitmap2(part.first_sector))
		return -1;
		
	int res = setBitmap2(BM_INODE, id, 1);
	
	if (closeBitmap2())
		return -1;
	
	return res;
}

int deallocate_inode(int id) {
	if (!is_mounted()) return -1;
	
	if ( id > part.max_inode_id)
		return -1;
		
	if(openBitmap2(part.first_sector))
		return -1;
		
	if (getBitmap2(BM_INODE, id) == 0)
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
	if (!is_mounted()) return -1;
	
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
	
	memcpy((void*)&buffer[inodeid_in_sector(dentry.inodeNumber)*sizeof(INODE2)], (void*)&inode, sizeof(INODE2));
	if (write_sector(inode_sector_id, buffer))
		return -1;
		
	return 0;
}

int load_inode(DIRENT2 dentry, INODE2* inode) {
	if (!is_mounted()) return -1;
	if (!inode) return -1;
	if (dentry.inodeNumber > part.max_inode_id)
		return -1;
	
	if(openBitmap2(part.first_sector))
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

int localize_freeblock() {
	if (!is_mounted()) return -1;
	if(openBitmap2(part.first_sector))
		return -1;
	
	int res = searchBitmap2(BM_BLOCK, 0);
	
	if(closeBitmap2())
		return -1;
		
	if (res == 0)
		return 0;
	if (res < 0)
		return -1;
	
	return res - part.first_block + 1;
}

int allocate_freeblock(int id) {
	if (!is_mounted()) return -1;
	
	if ( id > part.max_block_id)
		return -1;
		
	if(openBitmap2(part.first_sector))
		return -1;
		
	int res = setBitmap2(BM_BLOCK, part.first_block + (id - 1), 1);
	
	if (closeBitmap2())
		return -1;
	
	return res;
}

int deallocate_block(int id) {
	if (!is_mounted()) return -1;
	
	if ( id > part.max_block_id)
		return -1;
		
	if(openBitmap2(part.first_sector))
		return -1;
		
	int real_block_id = part.first_block + (id - 1);
	if (getBitmap2(BM_BLOCK, real_block_id) == 0)
		return -1;
		
	int res = setBitmap2(BM_BLOCK, real_block_id, 0);
	
	if (closeBitmap2())
		return -1;
	
	return res;
}

unsigned int blockid_to_sector(int id) {
	if (id <= 0)
		return -1;

	return part.first_block_sector + (id-1) * spb->blockSize;
}

BLOCKBUFFER new_block_buffer() {
	BLOCKBUFFER newbuffer = (BLOCKBUFFER) malloc(part.bytes_in_block);
	return newbuffer;
}

BLOCKBUFFER free_block_buffer(BLOCKBUFFER buffer) {
	if (!buffer)
		return NULL;
	
	free(buffer);
	buffer = NULL;
	return NULL;
}

int write_new_block(BLOCKBUFFER block, int* block_id) {
	*block_id = 0;
	if (!block || !is_mounted()) return -1;
	
	int new_id = localize_freeblock();
	if (new_id <= 0) {
		printf("Error allocating more disk space.\n");
		return -1;
	}
	
	if (allocate_freeblock(new_id))
		return -1;
	
	*block_id = new_id;

	SECTOR sector;
	unsigned int i;
	unsigned int sector_id;
	for (i = 0; i < spb->blockSize; i++) {
		memcpy((void*)sector, (void*)&block[i*SECTOR_SIZE], SECTOR_SIZE);
		sector_id = blockid_to_sector(*block_id);
		if (sector_id == -1)
			return -1;
		sector_id = sector_id + i;
		if(write_sector(sector_id, sector))
			return -1;
	}
	
	return 0;
}

int write_block(BLOCKBUFFER block, int block_id) {
	if (block_id <= 0 || block_id > part.max_block_id || !is_mounted())
		return -1;
		
	if (!block) return -1;
	
	if(openBitmap2(part.first_sector))
		return -1;
		
	int real_block_id = part.first_block + (block_id - 1);
	if (getBitmap2(BM_BLOCK, real_block_id) == 0)
		return -1;
	
	if (closeBitmap2()) return -1;
	
	SECTOR sector;
	unsigned int i;
	unsigned int sector_id;
	for (i = 0; i < spb->blockSize; i++) {
		memcpy((void*)sector, (void*)&block[i*SECTOR_SIZE], SECTOR_SIZE);
		sector_id = blockid_to_sector(block_id);
		if (sector_id == -1)
			return -1;
		sector_id = sector_id + i;
		if(write_sector(sector_id, sector))
			return -1;
	}
	
	return 0;
}

int load_block(BLOCKBUFFER block_buffer, int id) {
	if (id <= 0 || id > part.max_block_id || !is_mounted()) {
		return -1;
	}
	
	if (!block_buffer) return -1;
	
	if(openBitmap2(part.first_sector))
		return -1;
		
	int real_block_id = part.first_block + (id - 1);
	if (getBitmap2(BM_BLOCK, real_block_id) == 0)
		return -1;
	
	if (closeBitmap2()) return -1;
	
	SECTOR sector;
	unsigned int i;
	unsigned int sector_id;
	for (i = 0; i < spb->blockSize; i++) {
		sector_id = blockid_to_sector(id);
		if (sector_id == -1)
			return -1;
		sector_id = sector_id + i;
		if(read_sector(sector_id, sector))
			return -1;

		memcpy((void*)&block_buffer[i*SECTOR_SIZE], (void*)sector, SECTOR_SIZE);
	}
	
	return 0;
}

int blockid_sector_position(int block_pos) {
	return block_pos % _blockid_per_sector;
}

int block_single_indir_position(int block_pos) {
	if (!is_mounted()) return -1;
	
	return block_pos % part.blockids_in_block;
}

int block_double_indir_position(int block_pos) {
	if (!is_mounted()) return -1;
	int res = block_pos/part.blockids_in_block;

	if (res >= part.blockids_in_block) return -1;
	
	return res;
}

int create_filled_block(DWORD* storeId) {
	if (!storeId) return -1;

	int block_id;
	BLOCKBUFFER buffer = new_block_buffer();
	
	if (!buffer) return -1;
	
	SECTOR sector_buf;
	int i;
	for (i = 0; i < SECTOR_SIZE; i++) sector_buf[i] = -1; // Loading buffer with minus ones
	
	for (i = 0; i < spb->blockSize; i++)
		memcpy((void*)&buffer[i*SECTOR_SIZE], (void*)sector_buf, SECTOR_SIZE); // Copying minus ones to block buffer
		
	if (write_new_block(buffer, (int*)&block_id)) {
		free_block_buffer(buffer);
		return -1;
	}
	
	*storeId = block_id;
	free_block_buffer(buffer);
	
	return 0;
}

int write_to_single_indir_block(int block_pos, int indir_block_id, BLOCKBUFFER block) {
	if (!is_mounted()) return -1;
	
	if (!block) return -1;
	if (block_pos >= part.blockids_in_block) return -1;

	BLOCKBUFFER block_buffer = new_block_buffer();
	
	if (!block_buffer) return -1;
	
	DWORD block_id = 0;
	if (!load_block(block_buffer, indir_block_id)) { // Load indirection block
		int indir_block_pos = block_single_indir_position(block_pos);
		if (indir_block_pos != -1) { // Checking that the position is ok
			int* val = (int*) &block_buffer[indir_block_pos*sizeof(DWORD)];
			if (*val == -1) {
				
				if (!write_new_block(block, (int*)&block_id)) { // Write new block
					memcpy((void*)&block_buffer[indir_block_pos*sizeof(DWORD)],\
						   (void*)&block_id, sizeof(DWORD)); // Copying the block id to the indirection block
						   
					if (write_block(block_buffer, indir_block_id)) {
						deallocate_block(block_id); // Error: Deallocating the written block
						free_block_buffer(block_buffer);
						return -1;
					}
				} else {
					
					free_block_buffer(block_buffer);
					return -1;
				}
			} else {
				int* val = (int*) &block_buffer[indir_block_pos*sizeof(DWORD)];
				if (write_block(block, *val)) {
					return -1;
				}
			}
		} else {
			deallocate_block(block_id); // Error: Deallocating the written block
			free_block_buffer(block_buffer);
			return -1;
		}
	}
	
	free_block_buffer(block_buffer);
	return 0;
}

int write_to_double_indir_block(int block_pos, int indir_block_id, BLOCKBUFFER block) {
	if (!is_mounted()) return -1;
	
	if (!block) return -1;
	
	BLOCKBUFFER block_buffer1 = new_block_buffer();
	
	if (!block_buffer1) return -1;

	BLOCKBUFFER block_buffer2 = new_block_buffer();
	
	if (!block_buffer2) {
		free_block_buffer(block_buffer1);
		return -1;
	}

	int block_id = 0;
	if (!load_block(block_buffer1, indir_block_id)) { // Load double indirection block
		int db_indir_block_pos = block_double_indir_position(block_pos); // Return position of the singleIndirBlock
		int singleIndPtrExists = 0;
		
		if (db_indir_block_pos == -1) {
			free_block_buffer(block_buffer1);
			free_block_buffer(block_buffer2);
			return -1;
		}
	
		int val = *(int*) &block_buffer1[db_indir_block_pos*sizeof(DWORD)];
		if (val == -1) {  // Creating indirection block
			if (!create_filled_block((unsigned int*) &block_id)) {
				memcpy((void*)&block_buffer1[db_indir_block_pos*sizeof(DWORD)],\
					   (void*)&block_id, sizeof(DWORD)); // Copying the block id to the indirection block
				singleIndPtrExists = 1;
				
				if (write_block(block_buffer1, indir_block_id)) { // Writing to double indirection block
					free_block_buffer(block_buffer1);
					free_block_buffer(block_buffer2);
					return -1;
				}
			}
				
		} else {
			int* val2 = (int*) &block_buffer1[db_indir_block_pos*sizeof(DWORD)];
			block_id = *val2;
			singleIndPtrExists = 1;
		}

		if (singleIndPtrExists) { // Indirection block exists
		
			if (write_to_single_indir_block(block_pos%(part.blockids_in_block), block_id, block)) { // Writing content to single indirection block
				free_block_buffer(block_buffer1);
				free_block_buffer(block_buffer2);
				return -1;
			}
		}
	}
	
	free_block_buffer(block_buffer1);
	free_block_buffer(block_buffer2);
	return 0;
}

int write_inode_block(INODE2* inode, BLOCKBUFFER block, unsigned int block_pos) {
	if (!is_mounted()) return -1;
	if (!inode) return -1;
	if (!block) return -1;
	
	if (block_pos > inode->blocksFileSize) return -1;
	if (block_pos < 2) { // Direct Pointers
		switch(block_pos) {
			case 0:
				if (inode->dataPtr[0] == -1) {
					if(!write_new_block(block, (int*)&inode->dataPtr[0])) return 0;
					
				} else {
					if(!write_block(block, inode->dataPtr[0])) return 0;
				}
				break;
			case 1:
				if (inode->dataPtr[1] == -1) {
					if(!write_new_block(block, (int*)&inode->dataPtr[1])) return 0;
				} else {
					if(!write_block(block, inode->dataPtr[1])) return 0;
				}
				break;
		}
			
	} else if (block_pos < 2 + part.blockids_in_block) { // Single Indirection
		int singleIndPtrExists = 0;
		
		if (inode->singleIndPtr == -1) {  // Creating indirection block
		
			if (!create_filled_block(&inode->singleIndPtr))
				singleIndPtrExists = 1;
		} else singleIndPtrExists = 1;
		if (singleIndPtrExists) // Indirection block exists
		
			if (!write_to_single_indir_block(block_pos-2, inode->singleIndPtr, block))
				return 0;
		
	} else if (block_pos < 2 + part.blockids_in_block + part.blockids_in_block*part.blockids_in_block) { // Double indirection
		int doubleIndPtrExists = 0;
		if (inode->doubleIndPtr == -1) {  // Creating indirection block
			if (!create_filled_block(&inode->doubleIndPtr))
				doubleIndPtrExists = 1;
		} else doubleIndPtrExists = 1;
		
		if (doubleIndPtrExists) {
			if (!write_to_double_indir_block(block_pos-2-part.blockids_in_block, inode->doubleIndPtr, block)) {
				return 0;
			}
		}
	}
		
	return -1;
}

int append_block_to_inode(INODE2* inode, BLOCKBUFFER block) {
	if (!is_mounted()) return -1;
	if (!inode) return -1;
	if (!block) return -1;
	
	if(write_inode_block(inode, block, inode->blocksFileSize)) {
		return -1;
	}
	
	inode->blocksFileSize += 1;
	return 0;
}

int load_block_in_indir(int indir_block_id, BLOCKBUFFER buffer, int block_pos, unsigned int* block_id) {
	if (!buffer) return -1;
	if (!block_id) return -1;
	*block_id = 0;
	
	int block_pos_indir = block_single_indir_position(block_pos);
	
	if (block_pos_indir == -1) return -1;
	
	BLOCKBUFFER indir_buffer = new_block_buffer();
	if (!indir_buffer) return -1;
	
	if (!load_block(indir_buffer, indir_block_id)) {
		int block_id_ = *(int*) &indir_buffer[block_pos_indir*sizeof(DWORD)];
		*block_id = (unsigned int) block_id_;
		if (block_id_ == -1) {
			free_block_buffer(indir_buffer);
			return -1;
		}
		
		if (load_block(buffer, block_id_)) {
			free_block_buffer(indir_buffer);
			return -1;
		}
	}
	
	free_block_buffer(indir_buffer);
	return 0;
}

int load_block_in_db_indir(int indir_block_id, BLOCKBUFFER buffer, int block_pos, unsigned int* block_id) {
	if (!buffer) return -1;
	if (!block_id) return -1;
	*block_id = 0;
	
	int block_pos_indir = block_double_indir_position(block_pos);
	
	if (block_pos_indir == -1) return -1;
	
	BLOCKBUFFER indir_buffer = new_block_buffer();
	if (!indir_buffer) return -1;
	if (!load_block(indir_buffer, indir_block_id)) {
		int block_id_ = *(int*) &indir_buffer[block_pos_indir*sizeof(DWORD)];
		if (load_block_in_indir(block_id_, buffer, block_pos, block_id)) {
			free_block_buffer(indir_buffer);
			return -1;
		}
	} else {
		free_block_buffer(indir_buffer);
		return -1;
	}
	
	free_block_buffer(indir_buffer);
	return 0;
}

int load_inode_block(INODE2 inode, BLOCKBUFFER block_buffer, unsigned int block_pos, unsigned int* block_id) {
	if (!is_mounted()) return -1;
	if (!block_buffer) return -1;
	if (!block_id) return -1;
	
	
	if (block_pos > inode.blocksFileSize) return -1;
	*block_id = 0;

	if (block_pos < 2) { // Direct Pointers
		switch(block_pos) {
			case 0:
				if (inode.dataPtr[0] == -1) return -1;
				if(!load_block(block_buffer, inode.dataPtr[0])) {
					*block_id = inode.dataPtr[0];
					return 0;
				}
			case 1:
				if (inode.dataPtr[1] == -1) return -1;
				if(!load_block(block_buffer, inode.dataPtr[1])) {
					*block_id = inode.dataPtr[1];
					return 0;
				} 
		}
		return -1;
	} else if (block_pos < 2 + part.blockids_in_block) { // Single Indirection
	
		if (inode.singleIndPtr == -1) return -1;
		if (!load_block_in_indir(inode.singleIndPtr, block_buffer, block_pos-2, block_id)) return 0;
		
	} else if (block_pos < 2 + part.blockids_in_block + part.blockids_in_block*part.blockids_in_block) { // Double indirection
		if (inode.doubleIndPtr == -1) return -1;
		if (!load_block_in_db_indir(inode.doubleIndPtr, block_buffer, block_pos-2-part.blockids_in_block, block_id)) return 0;
	
	}
	
	return -1;
}

int remove_inode_content(INODE2* inode) {
	if (!inode) return -1;

	BLOCKBUFFER block_buffer = new_block_buffer();
	if (!block_buffer) return -1;
	
	unsigned int block_id = 0;
	int i;
	for (i = inode->blocksFileSize-1; i >= 0; i--) {
		if (load_inode_block(*inode, block_buffer, i, &block_id)) {
			free_block_buffer(block_buffer);
			return -1;
		}
		if (deallocate_block(block_id)) {
			free_block_buffer(block_buffer);
			return -1;
		}
		inode->blocksFileSize -= 1;
	}
	
	if (inode->doubleIndPtr != -1) {
		if (load_block(block_buffer, inode->doubleIndPtr)) {
			free_block_buffer(block_buffer);
			return -1;
		}
		for (i = 0; i < part.blockids_in_block; i++) {
			block_id = *(unsigned int*) &block_buffer[i*sizeof(DWORD)];
			if (block_id != -1) {
				if (deallocate_block(block_id)) {
					free_block_buffer(block_buffer);
					return -1;
				}
			}
		}
	}
	if (inode->singleIndPtr != -1) {
		if (deallocate_block(inode->singleIndPtr)) {
			free_block_buffer(block_buffer);
			return -1;
		}
	}
	
	free_block_buffer(block_buffer);
	inode->doubleIndPtr = -1;
	inode->singleIndPtr = -1;
	inode->dataPtr[1] = -1;
	inode->dataPtr[0] = -1;
	inode->blocksFileSize = 0;
	inode->bytesFileSize = 0;
	
	return 0;
}

int new_dentry(DIRENT2* dentry) {
	if (!dentry) return -1;
	
	dentry->TypeVal = 0x0;
	BYTE filename_[51] = {'\0'};
	memcpy(dentry->name, filename_, 51);
	
	if (write_new_inode(dentry)) return -1;
		
	return 0;
}

int empty_dentry(DIRENT2* dentry) {
	if (!dentry) return -1;
	
	dentry->TypeVal = 0x0;
	BYTE filename_[51] = {'\0'};
	memcpy(dentry->name, filename_, 51);
	
	dentry->inodeNumber = part.max_inode_id+1;
	
	return 0;
}

int delete_dentry(DIRENT2* dentry) {
	if (!dentry) return -1;
	
	INODE2 inode;
	if (load_inode(*dentry, &inode)) return -1;
	
	if (inode.RefCounter > 1) return -1;
	
	if (remove_inode_content(&inode)) return -1;
	
	if (deallocate_inode(dentry->inodeNumber)) return -1;	
	
	if (empty_dentry(dentry)) return -1;
	
	return 0;
}

int search_file_in_dir(char* filename, DIRENT2* dentry, int* dentry_block, int* dentry_pos) {
	if (!is_dir_open()) return -1;
	if (!dentry) return -1;
	if (!dentry_block) return -1;
	if (!dentry_pos) return -1;
	
	int curr_entry = 0;
	unsigned int block_pos;
	
	BLOCKBUFFER block_buffer = new_block_buffer();
	if (!block_buffer) return -1;
	
	unsigned int block_id;
	unsigned int last_bpos = -1;
	unsigned int pos_in_block;
	
	DIRENT2 dummyDentry;
	
	BYTE filename_[51];
	if (check_filename(filename_, (BYTE*)filename)) return -1;
	
	while(curr_entry*sizeof(DIRENT2) < thedir->inode.bytesFileSize) {
		block_pos = curr_entry / part.dirs_in_block;
	
		if (block_pos != last_bpos) {
			if (load_inode_block(thedir->inode, block_buffer, block_pos, &block_id)) return -1;
		}
		
		pos_in_block = curr_entry % part.dirs_in_block;
		
		memcpy((void*)&dummyDentry, (void*)&block_buffer[pos_in_block*sizeof(DIRENT2)], sizeof(DIRENT2));
		
		if (strcmp((char*)filename_, dummyDentry.name) == 0) {
			*dentry_block = block_pos;
			*dentry_pos = pos_in_block;
			free_block_buffer(block_buffer);
			memcpy((void*)dentry, (void*)&dummyDentry, sizeof(DIRENT2));
			return 0;
		}
		curr_entry += 1;
	}
	
	free_block_buffer(block_buffer);
	
	return -1;
}

int write_to_invalid_dentry_in_dir(DIRENT2 dentry) {
	if (!is_dir_open()) return -1;
	
	int curr_entry = 0;
	unsigned int block_pos;
	
	BLOCKBUFFER block_buffer = new_block_buffer();
	if (!block_buffer) return -1;
	
	unsigned int block_id;
	unsigned int last_bpos = -1;
	unsigned int pos_in_block;
	
	DIRENT2 dummyDentry;
	while(curr_entry*sizeof(DIRENT2) < thedir->inode.bytesFileSize) {
		block_pos = curr_entry / part.dirs_in_block;
		
	
		if (block_pos != last_bpos) {
			if (load_inode_block(thedir->inode, block_buffer, block_pos, &block_id)) return -1;
		}
		
		pos_in_block = (curr_entry % part.dirs_in_block)*sizeof(DIRENT2);
				
		memcpy((void*)&dummyDentry, (void*)&block_buffer[pos_in_block], sizeof(DIRENT2));
		
		if (dummyDentry.TypeVal == 0x0) {
			memcpy((void*)&block_buffer[pos_in_block], (void*)&dentry, sizeof(DIRENT2));
			
			if (write_block(block_buffer, block_id)) {
				free_block_buffer(block_buffer);
				return -1;
			}
			
			free_block_buffer(block_buffer);
			return 0;
		}
		curr_entry += 1;
	}
	
	free_block_buffer(block_buffer);
	
	return -1;
}

int write_dentry_to_dir(DIRENT2 dentry) {
	if (!is_dir_open()) return -1;
	
	unsigned int dir_size_bytes = thedir->inode.bytesFileSize;
	unsigned int dir_size_blocks = thedir->inode.blocksFileSize;
	unsigned int last_block_pos = dir_size_blocks - 1;

	unsigned int bid;

	INODE2* inode = &(thedir->inode);
	if (dir_size_bytes < part.max_dentries*sizeof(DIRENT2)) { // Append
		BLOCKBUFFER buffer = new_block_buffer();
		if (!buffer) return -1;
		
		if ((dir_size_bytes / (spb->blockSize*SECTOR_SIZE)) < dir_size_blocks) { // In Existing Block
			if (load_inode_block(thedir->inode, buffer, last_block_pos, &bid)) {
				free_block_buffer(buffer);
				return -1;
			}
			
			memcpy((void*)&buffer[dir_size_bytes % (spb->blockSize*SECTOR_SIZE)], (void*)&dentry, sizeof(DIRENT2));

			if (write_block(buffer, bid)) {
				free_block_buffer(buffer);
				return -1;
			}
			
			free_block_buffer(buffer);
			inode->bytesFileSize = thedir->inode.bytesFileSize + sizeof(DIRENT2);

			return 0;
						
		} else { // In New Block
			memcpy((void*)buffer, (void*)&dentry, sizeof(DIRENT2));
			if (append_block_to_inode(&(thedir->inode), buffer)) {
				free_block_buffer(buffer);
				return -1;
			}
			
			free_block_buffer(buffer);
			
			inode->bytesFileSize += sizeof(DIRENT2);
			return 0;
			
		}
		
	} else { // Search for invalid dentry and write
		if (!write_to_invalid_dentry_in_dir(dentry)) {
			return 0;
		}
	}
	
	return -1;
}
