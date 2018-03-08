/**
* @brief Ejercicio 13b
* Queremos crear dos hilos para que multipliquen simultaneamente una 
* matriz por un entero cada uno, y que se comuniquen usando variables
* globales de forma que cada hilo sepa por donde va el otro.
*
* @file Ejercicio13b.c
* @author Javier.delgadod@estudiante.uam.es Javier.lopezcano@estudiante.uam.es
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int pos1, pos2; /*!< Variables globales usadas para la comunicacion entre hilos*/

/**
* @brief Estructura para representar una matriz cuadrada
* de hasta cinco por cinco enteros.
*
* Compuesta por un doble array y un entero que indica su tamaño.
*/
typedef struct _matriz{
	int matriz[5][5]; /*!< Array cuadrado*/
	int size;/*!< Tamaño del array*/
}matriz;


/**
* @brief Estructura para pasar como argumento a los distintos hilos.
* 
* Compuesta por una matriz y un entero que indica el numero de hilo,
* y otro que indica el numero por el que queremos multiplicar la matriz.
*/
typedef struct _multiplicacion{
	matriz mat;
	int multiplicador;
	int hilo;
}multiplicacion;


/**
* @brief multiplica una matriz cuadrada por un entero.
* 
* multiplicarMatriz(void *args) mutiplica una matriz por un numero
* fila por fila, imprimiendo el resultado y el numero del hilo.
* 
* @param args un puntero a multiplicacion pasado como puntero a void.
* en el se pasa la matriz y el multiplicador.
* @return void* 0 siempre a NULL.
*/
void* multiplicarMatriz(void *args){
	int i, j;
	char text[1024];
	char temp[256];
	multiplicacion *mul = (multiplicacion *) args;

	for(i = 0; i < mul->mat.size; i++){
		/*Introducimos retardos para que se vea el paralelismo*/
		usleep (1000000);
		sprintf(text, "Hilo %d multiplicando fila %d. Resultado: ", mul->hilo, i);
		for(j = 0; j < mul->mat.size; j++){
			mul->mat.matriz[i][j] *= mul->multiplicador;
			sprintf(temp, "%d ", mul->mat.matriz[i][j]);
			strcat(text, temp);
		}
		if(mul->hilo == 1){
			pos1++;
			sprintf(temp, "- el Hilo 2 va por la fila %d\n", pos2);
		}else if(mul->hilo == 2){
			pos2++;
			sprintf(temp, "- el Hilo 1 va por la fila %d\n", pos1);
		}
		printf("%s%s\n", text, temp);
	}
	return NULL;
}

/**
* @brief crea dos hilos para multiplicar dos matrices por dos enteros
* respectivamente.
* 
* main (int argc, char* argv[]) obtiene dos matrices cuadradas y dos
* multiplicadores mediante la entrada del teclado, y crea un hilo
* para mutiplicar cada una de estas matrices por el multiplicador.
* @param argc numero de argumentos recibidos en la entrada.
* @param argv array de strings de argumentos recibidos en la entrada.
* 
* @return int que determina si el programa se ha ejecutado o no con exito.
*/
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
		scanf("%d", &(mat.matriz[i/mat.size][i%mat.size]));
	}
	printf("Introduzca matriz 2:\n");
	for(i = 0; i < mat2.size*mat2.size; i++){
		scanf("%d", &(mat2.matriz[i/mat2.size][i%mat2.size]));
	}

	mu1.mat = mat;
	mu1.hilo = 1;
	mu2.mat = mat2;
	mu2.hilo = 2;
	pthread_create(&h[0], NULL , multiplicarMatriz , &mu1); 
	pthread_create(&h[1], NULL , multiplicarMatriz , &mu2); 	

	pthread_join(h[0],NULL);
	pthread_join(h[1],NULL); 

	exit(EXIT_SUCCESS);
}