#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main(){
	FILE *fp;
	fp = fopen("/mnt/zofs/gcc.log", "w+");
	if(fp == NULL){
		perror("fopen fail");
		return 0;
	}
	int r = fclose(fp);
	if(r != 0){
		strerror(errno);
	}
}
