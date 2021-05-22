#include <string.h>
#include <stdio.h>

int main(){
	char ini[100], itu[100];
	scanf("%s", ini);
	scanf("%s", itu);
	if(strcmp(ini, "apa") == 0) printf("%s\n", itu);
}