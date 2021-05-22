#include<stdio.h>
#include <stdlib.h>

int main(){
	FILE *fp = fopen("/home/prabu/Desktop/inifile.txt", "rb");
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *string = malloc(fsize+1);
	fread(string, 1, fsize, fp);
	fclose(fp);

	puts(string);
}