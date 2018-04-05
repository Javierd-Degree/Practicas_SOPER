#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

/*Para compartir informacion entre hilos, usamos dos variables globales.*/
int pos1, pos2;


typedef struct _matriz{
	int matriz[5][5];
	int size;
}matriz;

typedef struct _multiplicacion{
	matriz mat;
	int multiplicador;
	int hilo;
}multiplicacion;

void* multiplicarMatriz(void *args){
	int i, j;
	multiplicacion *mul = (multiplicacion *) args;

	for(i = 0; i < mul->mat.size; i++){
		/*Introducimos retardos para que se vea el paralelismo*/
		usleep (1000000);
		printf("Hilo %d multiplicando fila %d. Resultado: ", mul->hilo, i);
		for(j = 0; j < mul->mat.size; j++){
			mul->mat.matriz[i][j] *= mul->multiplicador;
			printf("%d ", mul->mat.matriz[i][j]);
		}
		if(mul->hilo == 1){
			pos1++;
			printf("- el Hilo 2 va por la fila %d\n", pos2);
		}else if(mul->hilo == 2){
			pos2++;
			printf("- el Hilo 1 va por la fila %d\n", pos1);
		}
		
	}

	return NULL;
}

int main(int argc , char *argv[]) { 	
	matriz mat, mat2;
	multiplicacion mu1, mu2;
	int i;
	pthread_t h[2];

	do{
		printf("Introduzca dimension de la matriz cuadrada: ");
		scanf("%d", &(mat.size));
		mat2.size = mat.size;
	}while(mat.size > 5);
	printf("Introduzca multiplicador 1: ");
	scanf("%d", &(mu1.multiplicador));
	printf("Introduzca multiplicador 2: ");
	scanf("%d", &(mu2.multiplicador));

	/*Rellenamos las matrices*/
	printf("Introduzca matriz 1:\n");
	for(i = 0; i < mat.size*mat.size; i++){
		scanf("%d", &(mat.matriz[i%mat.size][i/mat.size]));
	}
	printf("Introduzca matriz 2:\n");
	for(i = 0; i < mat2.size*mat2.size; i++){
		scanf("%d", &(mat2.matriz[i%mat2.size][i/mat2.size]));
	}

	mu1.mat = mat;
	mu1.hilo = 1;
	mu2.mat = mat;
	mu2.hilo = 2;
	pos1 = pos2 = 0;
	pthread_create(&h[0], NULL , multiplicarMatriz , &mu1); 
	pthread_create(&h[1], NULL , multiplicarMatriz , &mu2); 	

	pthread_join(h[0],NULL);
	pthread_join(h[1],NULL); 

	exit(EXIT_SUCCESS);
}