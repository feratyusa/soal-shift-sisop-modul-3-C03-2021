#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#define MAX 50000
#define DATA_BUFFER 1000

void take_file_name(char filename[], char file[]){
	char *check, file_temp[DATA_BUFFER], str[100];
	strcpy(file_temp, file);
	check = strtok(file_temp, "/");
	while(check != NULL)
	{
		strcpy(str, check);
		check = strtok(NULL, "/");
	}
	strcpy(filename, str);
}

void see_file()
{
	char deskripsi_total[MAX], file[MAX];
	strcpy(deskripsi_total, "\n");
	FILE *fp = fopen("files.tsv", "r");
	while(fgets(file, sizeof(file), fp)){
		// Deskripsi sementara
		char deskripsi[MAX], *check, file_temp[MAX], filepath[100], name[100], ekstensi[100], publisher[100], tahun[100];
		

		// Ambil filename dan ekstensi
		strcpy(file_temp, file);
		char filename[1000], *dot;
		check = strtok(file_temp, "\t");
		strcpy(filepath, check);
		take_file_name(filename, check);
		dot = strtok(filename, ".");
		strcpy(name, dot);
		dot = strtok(NULL, ".");
		strcpy(ekstensi, dot);

		// Ambil publisher
		strcpy(file_temp, file);
		check = strtok(file_temp, "\t");
		check = strtok(NULL, "\t");
		strcpy(publisher, check);

		// Ambile Tahun
		check = strtok(NULL, "\t");
		strcpy(tahun, check);

		// Gabung
		sprintf(deskripsi, "Nama: %s\nPublisher: %s\nTahun Publishing: %sEkstensi File: %s\nFilepath: %s\n\n", name, publisher, tahun, ekstensi, filepath);

		// Gabung Total
		strcat(deskripsi_total, deskripsi);
	}
	fclose(fp);

	printf("%s\n", deskripsi_total);
}

int main()
{
	printf("Mulai\n");
	see_file();
}