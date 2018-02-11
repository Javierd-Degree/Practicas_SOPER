#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>


void main(){
	FILE* f;
	char* path = "./prueba.txt", *text="texto prueba", buffer[512];
	int handle;

	handle = open(path, O_WRONLY);

	int w = write(handle, text, strlen(text)+1);
	if( w < strlen(text)){
		printf("Error al escribir\n");
		return;
	}

	close(handle);
	handle = open(path, O_RDONLY);

	int r = read(handle, buffer, strlen(text)+1);
	if( r < strlen(text)){
		printf("Error al leer\n");
		return;
	}

	printf("%s\n", buffer);
	close(handle);

	f= fopen(path, "a");
	fgets(buffer, strlen(text), f);
	printf("%s\n", buffer);

	fclose(f);
}