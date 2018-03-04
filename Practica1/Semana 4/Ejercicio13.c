#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

typedef struct _matriz{
	int **matriz;
	int size;
}matriz;


int main(int argc , char *argv[]) { 	
	matriz mat;
	int i, j;
	int mult1, mult2;

	printf("Introduzca dimension de la matriz cuadrada:\n");
	scanf("\n%d", &(mat.size));
	printf("Introduzca multiplicador 1:\n");
	scanf("\n%d", &mult1);
	printf("Introduzca multiplicador 2:\n");
	scanf("\n%d", &mult2);

	mat.size = atoi(argv[1]);
	mat.matriz = (int **)malloc(sizeof(int*)*mat.size);
	if(mat.matriz == NULL){
		printf("No hay suficiente memoria.\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < mat.size; i++){
		mat.matriz[i] = (int *)malloc(sizeof(int)*mat.size);
		if(mat.matriz[i] == NULL){
			for(j = 0; j < i; j++){
				free(mat.matriz[j]);
			}
			free(mat.matriz);
			printf("No hay suficiente memoria.\n");
			exit(EXIT_FAILURE);
		}
	}

	
	for(i = 0; i < mat.size; i++){
		free(mat.matriz[i]);
	}
	free(mat.matriz);

	exit(EXIT_SUCCESS);
}