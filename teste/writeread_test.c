
#include <stdio.h>
#include <t2fs.h>

typedef struct s_partition PARTITION;

extern PARTITION part;

int main() {
	if (format2(1, 1)) {
		printf("Error while formatting 1: NOT OK\n");
		return -1;
	}
	if (mount(1)) {
		printf("Error while mounting 1: NOT OK\n");
		return -1;
	}
	
	FILE2 fhandler = create2("my_file");
	printf("Creating file: %s\n", fhandler >= 0 ? "OK" : "NOT OK");
	
	
	char buffer[] = {"\nERA UMA VEZ UM GATO XADREZ, QUER QUE EU TE CONTE OUTRA VEZ?\nMARIAZINHA FOI NO PARQUE CAÇAR BORBOLETA, TROPEÇOU NUM ARBUSCO E RASGOU A SACOLINHA \
QUE A VÓ DELA TINHA FEITO PRA ELA A COITADA\nERA UMA VEZ UM GATO XADREZ, QUER QUE EU TE CONTE OUTRA VEZ?\nMARIAZINHA FOI NO PARQUE CAÇAR BORBOLETA, TROPEÇOU NUM ARBUSCO E RASGOU A SACOLINHA \
QUE A VÓ DELA TINHA FEITO PRA ELA A COITADA\nERA UMA VEZ UM GATO XADREZ, QUER QUE EU TE CONTE OUTRA VEZ?\nMARIAZINHA FOI NO PARQUE CAÇAR BORBOLETA, TROPEÇOU NUM ARBUSCO E RASGOU A SACOLINHA \
QUE A VÓ DELA TINHA FEITO PRA ELA A COITADA\n"};

	char buffer2[sizeof(buffer)*2];
	int i;
	for (i = 0; i < sizeof(buffer)*2; i++) buffer2[i] = '\0';
	
	printf("Printing write buffer: \n%s\n\n", buffer);
		
	printf("Writing to file: %s\n", write2(fhandler, buffer, sizeof(buffer)) > 0 ? "OK" : "NOT OK");
	
	printf("Closing file: %s\n", close2(fhandler) == 0 ? "OK" : "NOT OK");
	
	fhandler = open2("my_file");
	printf("Openning file: %s\n", fhandler >= 0 ? "OK" : "NOT OK"); 
	printf("Reading from file: %s\n", read2(fhandler, buffer2, sizeof(buffer2)) > 0 ? "OK" : "NOT OK");
	
	printf("Writing to file: %s\n", write2(fhandler, buffer, sizeof(buffer)) > 0 ? "OK" : "NOT OK");

	printf("Closing file: %s\n", close2(fhandler) == 0 ? "OK" : "NOT OK");
	
	fhandler = open2("my_file");
	printf("Openning file: %s\n", fhandler >= 0 ? "OK" : "NOT OK"); 
	printf("Reading from file: %s\n", read2(fhandler, buffer2, sizeof(buffer2)) > 0 ? "OK" : "NOT OK");
	
	buffer2[sizeof(buffer)-1] = '0';
	printf("\nPrinting read buffer: \n%s\n\n", buffer2);
	
	printf("Closing file: %s\n", close2(fhandler) == 0 ? "OK" : "NOT OK");
	
	printf("\n\n\n");
	
	char buffer3[] = {"UM MENINO FOI BUSCAR LENHA NA FLORESTA COM SEU BURRICO E LEVOU JUNTO SEU CACHORRO ESTIMAÇÃO. CHEGANDO NO MEIO DA MATA, O MENINO JUNTOU UM GRANDE FEIXE DE LENHA, OLHOU PARA O BURRO, E EXCLAMOU: VOU COLOCAR UMA CARGA DE LENHA DE LASCAR NESSE JUMENTO !!!!\n"};
	char buffer4[sizeof(buffer3)*2];
	for (i = 0; i < sizeof(buffer3)*2; i++) buffer4[i] = '\0';
	
	fhandler = create2("my_file2");
	printf("Creating file: %s\n", fhandler >= 0 ? "OK" : "NOT OK");
	
	printf("Printing write buffer: \n%s\n\n", buffer3);
		
	printf("Writing to file: %s\n", write2(fhandler, buffer3, sizeof(buffer3)) > 0 ? "OK" : "NOT OK");
	
	printf("Closing file: %s\n", close2(fhandler) == 0 ? "OK" : "NOT OK");
	
	fhandler = open2("my_file2");
	printf("Openning file: %s\n", fhandler >= 0 ? "OK" : "NOT OK"); 
	printf("Reading from file: %s\n", read2(fhandler, buffer4, sizeof(buffer3)) > 0 ? "OK" : "NOT OK");
	
	printf("\nPrinting read buffer: \n%s\n\n", buffer4);
	
	printf("Writing to file: %s\n", write2(fhandler, buffer3, sizeof(buffer3)) > 0 ? "OK" : "NOT OK");

	printf("Closing file: %s\n", close2(fhandler) == 0 ? "OK" : "NOT OK");
	
	printf("\n\n\n");
	
	fhandler = open2("my_file2");
	printf("Openning file: %s\n", fhandler >= 0 ? "OK" : "NOT OK"); 
	printf("Reading from file: %s\n", read2(fhandler, buffer4, sizeof(buffer4)) > 0 ? "OK" : "NOT OK");
	
	buffer2[sizeof(buffer3)-1] = '0';
	printf("\nPrinting read buffer: \n%s\n\n", buffer4);
	
	printf("Closing file: %s\n", close2(fhandler) == 0 ? "OK" : "NOT OK");
	
	fhandler = open2("my_file2");
	printf("Openning file: %s\n", fhandler >= 0 ? "OK" : "NOT OK"); 
	int readsize = read2(fhandler, buffer4, sizeof(buffer3));
	printf("%d\n", sizeof(buffer3));
	printf("Reading from file - %d: %s\n", readsize, readsize  > 0 ? "OK" : "NOT OK");
	
	printf("\nPrinting read buffer: \n%s\n\n", buffer4);
	int written =  write2(fhandler, buffer, sizeof(buffer3));
	printf("Writing to file - written: %d %s\n", written, written > 0 ? "OK" : "NOT OK");

	printf("Closing file: %s\n", close2(fhandler) == 0 ? "OK" : "NOT OK");
	
	fhandler = open2("my_file2");
	printf("Openning file: %s\n", fhandler >= 0 ? "OK" : "NOT OK"); 
	printf("Reading from file: %s\n", read2(fhandler, buffer2, sizeof(buffer2)) > 0 ? "OK" : "NOT OK");
	
	buffer2[sizeof(buffer3)-1] = '0';
	printf("\nPrinting read buffer: \n%s\n\n", buffer2);
	
	printf("Closing file: %s\n", close2(fhandler) == 0 ? "OK" : "NOT OK");
	
	if(umount()) {
		printf("Error while unmounting 1: NOT OK\n");
		return -1;
	}

	return 0;
}
