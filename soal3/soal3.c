#include<stdio.h>
#include<stdlib.h> 
#include<string.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>
#include<errno.h>
#include<ctype.h>

//pthread_mutex_t signal;

char *checkName(char *dir)
{
	char *name = strrchr(dir, '/');
	if(name == dir) 
		return "";
	return name + 1;
}

char *lowercase(char *str)
{
	unsigned char *temp = (unsigned char *)str;
	while(*temp)
	{
		*temp = tolower(*temp);
		temp++;
	}
	return str;
}

char *checkExt(char *dir)
{
	char *tmp = strrchr(dir, '/');
	int i = 0;	
	char ext[400];
	int j = i;
	while(i < strlen(tmp)) ext[i-j] = tmp[i], i++;
	return lowercase(ext + 1);
}

void* categorize(void *arg)
{
	char *src = (char *)arg;
	char srcP[150];
	memcpy(srcP, (char*)arg, 400);
	char *srcExt = checkExt(src);
	char ext[400];
	strcpy(ext, srcExt);
	
	DIR *dir = opendir(srcExt);
	if(dir) 
		closedir(dir);
	else if(ENOENT == errno) 
		mkdir(srcExt, 0755);
	
	char *srcName = checkName(srcP);
	char *curr = getenv("PWD");
	
	char destP[1000];
	sprintf(destP, "%s/%s/%s", curr, ext, srcName);
	
	rename(srcP, destP);
}

int main(int argc, char *argv[])
{
	if(argc == 0) 
		exit(EXIT_FAILURE);
	if(argc > 2 && strcmp(argv[1], "-f") == 0)
	{
		pthread_t tid[argc-2];
		int count = 0;
		for(int i=2; i<argc; i++)
		{
			if(access(argv[i], F_OK) == 0)
			{
				pthread_create(&tid[count], NULL, categorize, (void *)argv[i]);
				count++;
				printf("File %d : Berhasil Dikategorikan\n", i-1);
			}
			else printf("File %d : Sad, gagal :(\n", i-1);
		}
		for(int i=0; i<count; i++) 
			pthread_join(tid[i], NULL);
		return 0;
	}
}
	
