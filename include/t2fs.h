

#ifndef __LIBT2FS___
#define __LIBT2FS___


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <bitmap2.h>
#include <apidisk.h>
#include <support.h>

#define	TYPEVAL_INVALIDO	0x00
#define	TYPEVAL_REGULAR		0x01
#define	TYPEVAL_LINK		0x02

typedef int FILE2;

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int DWORD;


#pragma pack(push, 1)

#define MAX_FILE_NAME_SIZE 255
typedef struct s_dirent {
    char    name[MAX_FILE_NAME_SIZE+1]; /* Nome do arquivo cuja entrada foi lida do disco      */
    BYTE    fileType;                   /* Tipo do arquivo: regular (0x01) ou diretório (0x02) */
    DWORD   fileSize;                   /* Numero de bytes do arquivo                          */
} DIRENT2;

#pragma pack(pop)


/*-----------------------------------------------------------------------------
Função: Usada para identificar os desenvolvedores do T2FS.
	Essa função copia um string de identificação para o ponteiro indicado por "name".
	Essa cópia não pode exceder o tamanho do buffer, informado pelo parâmetro "size".
	O string deve ser formado apenas por caracteres ASCII (Valores entre 0x20 e 0x7A) e terminado por ‘\0’.
	O string deve conter o nome e número do cartão dos participantes do grupo.

Entra:	name -> buffer onde colocar o string de identificação.
	size -> tamanho do buffer "name" (número máximo de bytes a serem copiados).

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int identify2 (char *name, int size);


/*-----------------------------------------------------------------------------
Função:	Formata uma partição do disco virtual.
		Uma partição deve ser montada, antes de poder ser montada para uso.

Entra:	partition -> número da partição a ser formatada
		sectors_per_block -> número de setores que formam um bloco, para uso na formatação da partição

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int format2(int partition, int sectors_per_block);


/*-----------------------------------------------------------------------------
Função:	Monta a partição indicada por "partition" no diretório raiz

Entra:	partition -> número da partição a ser montada

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int mount(int partition);


/*-----------------------------------------------------------------------------
Função:	Desmonta a partição atualmente montada, liberando o ponto de montagem.

Entra:	-

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int umount(void);


/*-----------------------------------------------------------------------------
Função: Criar um novo arquivo.
	O nome desse novo arquivo é aquele informado pelo parâmetro "filename".
	O contador de posição do arquivo (current pointer) deve ser colocado na posição zero.
	Caso já exista um arquivo com o mesmo nome, a função deverá retornar um erro de criação.
	A função deve retornar o identificador (handle) do arquivo.
	Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado.

Entra:	filename -> nome do arquivo a ser criado.

Saída:	Se a operação foi realizada com sucesso, a função retorna o handle do arquivo (número positivo).
	Em caso de erro, deve ser retornado um valor negativo.
-----------------------------------------------------------------------------*/
FILE2 create2 (char *filename);


/*-----------------------------------------------------------------------------
Função:	Apagar um arquivo do disco.
	O nome do arquivo a ser apagado é aquele informado pelo parâmetro "filename".

Entra:	filename -> nome do arquivo a ser apagado.

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int delete2 (char *filename);


/*-----------------------------------------------------------------------------
Função:	Abre um arquivo existente no disco.
	O nome desse novo arquivo é aquele informado pelo parâmetro "filename".
	Ao abrir um arquivo, o contador de posição do arquivo (current pointer) deve ser colocado na posição zero.
	A função deve retornar o identificador (handle) do arquivo.
	Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado.
	Todos os arquivos abertos por esta chamada são abertos em leitura e em escrita.
	O ponto em que a leitura, ou escrita, será realizada é fornecido pelo valor current_pointer (ver função seek2).

Entra:	filename -> nome do arquivo a ser apagado.

Saída:	Se a operação foi realizada com sucesso, a função retorna o handle do arquivo (número positivo)
	Em caso de erro, deve ser retornado um valor negativo
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename);


/*-----------------------------------------------------------------------------
Função:	Fecha o arquivo identificado pelo parâmetro "handle".

Entra:	handle -> identificador do arquivo a ser fechado

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle);


/*-----------------------------------------------------------------------------
Função:	Realiza a leitura de "size" bytes do arquivo identificado por "handle".
	Os bytes lidos são colocados na área apontada por "buffer".
	Após a leitura, o contador de posição (current pointer) deve ser ajustado para o byte seguinte ao último lido.

Entra:	handle -> identificador do arquivo a ser lido
	buffer -> buffer onde colocar os bytes lidos do arquivo
	size -> número de bytes a serem lidos

Saída:	Se a operação foi realizada com sucesso, a função retorna o número de bytes lidos.
	Se o valor retornado for menor do que "size", então o contador de posição atingiu o final do arquivo.
	Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
int read2 (FILE2 handle, char *buffer, int size);


/*-----------------------------------------------------------------------------
Função:	Realiza a escrita de "size" bytes no arquivo identificado por "handle".
	Os bytes a serem escritos estão na área apontada por "buffer".
	Após a escrita, o contador de posição (current pointer) deve ser ajustado para o byte seguinte ao último escrito.

Entra:	handle -> identificador do arquivo a ser escrito
	buffer -> buffer de onde pegar os bytes a serem escritos no arquivo
	size -> número de bytes a serem escritos

Saída:	Se a operação foi realizada com sucesso, a função retorna o número de bytes efetivamente escritos.
	Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
int write2 (FILE2 handle, char *buffer, int size);


/*-----------------------------------------------------------------------------
Função:	Abre o diretório raiz da partição ativa.
		Se a operação foi realizada com sucesso, 
		a função deve posicionar o ponteiro de entradas (current entry) na primeira posição válida do diretório.

Entra:	-

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int opendir2 (void);


/*-----------------------------------------------------------------------------
Função:	Realiza a leitura das entradas do diretório aberto
		A cada chamada da função é lida a entrada seguinte do diretório
		Algumas das informações dessas entradas devem ser colocadas no parâmetro "dentry".
		Após realizada a leitura de uma entrada, o ponteiro de entradas (current entry) será ajustado para a  entrada válida seguinte.
		São considerados erros:
			(a) qualquer situação que impeça a realização da operação
			(b) término das entradas válidas do diretório aberto.

Entra:	dentry -> estrutura de dados onde a função coloca as informações da entrada lida.

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero ( e "dentry" não será válido)
-----------------------------------------------------------------------------*/
int readdir2 (DIRENT2 *dentry);


/*-----------------------------------------------------------------------------
Função:	Fecha o diretório identificado pelo parâmetro "handle".

Entra:	-

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
		Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int closedir2 (void);


/*-----------------------------------------------------------------------------
Função:	Cria um link simbólico (soft link)

Entra:	linkname -> nome do link
		filename -> nome do arquivo apontado pelo link

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int sln2(char *linkname, char *filename);


/*-----------------------------------------------------------------------------
Função:	Cria um link estrito (hard link)

Entra:	linkname -> nome do link
		filename -> nome do arquivo apontado pelo link

Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero).
	Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int hln2(char *linkname, char *filename);

/*------------------------------- FIM DA API --------------------------------*/ 

typedef struct s_mbr MBR;
typedef struct t2fs_superbloco SUPERBLOCK;
typedef struct t2fs_record DENTRY2;
typedef struct t2fs_inode INODE2;
typedef struct s_thedir THEDIR;
typedef struct s_swofl_entry SWOFL_ENTRY;
typedef struct s_pwofl_entry PWOFL_ENTRY;
typedef struct s_partition PARTITION;

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

/** Superbloco  - 19/2 */
typedef struct t2fs_superbloco {
	char    id[4];					/** "T2FS" */
	WORD    version;				/** 0x7E32 */
	WORD    superblockSize;			/** 1 = Número de blocos ocupados pelo superbloco */
	WORD	freeBlocksBitmapSize;	/** Número de blocos do bitmap de blocos de dados */
	WORD	freeInodeBitmapSize;	/** Número de blocos do bitmap de i-nodes */
	WORD	inodeAreaSize;			/** Número de blocos reservados para os i-nodes */
	WORD	blockSize;				/** Número de setores que formam um bloco */
	DWORD	diskSize;				/** Número total de blocos da partição */
	DWORD	Checksum;				/** Soma dos 5 primeiros inteiros de 32 bits do superbloco */
} SUPERBLOCK;

typedef struct t2fs_record {
	BYTE    TypeVal;
	char    name[51];
	DWORD	Nao_usado[2];
	DWORD	inodeNumber;
} DENTRY2;


/** i-node - 19/2 */
typedef struct t2fs_inode {
	DWORD	blocksFileSize;
	DWORD	bytesFileSize;
	DWORD	dataPtr[2];
	DWORD	singleIndPtr;
	DWORD	doubleIndPtr;
	DWORD	RefCounter;
	DWORD	reservado;
} INODE2;


typedef struct s_thedir {
	INODE2 inode;
	unsigned int current_entry;
} THEDIR;


typedef struct s_swofl_entry {
	DENTRY2* dir_entry;
	unsigned int refs;
	NODE2* swofl_container;
} SWOFL_ENTRY;


typedef struct s_pwofl_entry {
	FILE2 id;
	SWOFL_ENTRY* sys_file;
	unsigned int current_position;
	NODE2* pwofl_container;
} PWOFL_ENTRY;


typedef struct s_partition {
	unsigned int first_sector;
	unsigned int first_inodeblock;
	unsigned int first_inode_sector;
	unsigned int first_block;
	unsigned int first_block_sector;
	unsigned int max_inode_id;
	unsigned int max_block_id;
	unsigned int bytes_in_block;
	unsigned int blockids_in_block;
	unsigned int dirs_in_block;
	unsigned int max_dentries;
	unsigned int max_blocks_in_inode;
	int dir_open;
} PARTITION;


typedef BYTE SECTOR[SECTOR_SIZE];
typedef BYTE* BLOCKBUFFER;

#pragma pack(pop)

#endif

