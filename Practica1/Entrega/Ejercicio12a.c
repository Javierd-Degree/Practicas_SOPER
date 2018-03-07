/**
* @brief Apartado a del ejercicio 12
* Queremos medir el tiempo aproximdo que se tarda en crear y destruir
* cien procesos, y en que cada uno de estos calcule los N primeros 
* numeros primos, donde N es un parametro introducido por el usuario.
*
* @file Ejercicio12a.c
* @author Javier.delgadod@estudiante.uam.es Javier.lopezcano@estudiante.uam.es
*/

#include <stdio.h> 	
#include <stdlib.h> 	
#include <string.h>
#include <math.h> 
#include <unistd.h> 	
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h> 

/**
* @brief Estructura para guardar un texto aleatorio y el 
* numero N de primos que tenemos que calcular.
*
* Compuesta con un string de 100 caracteres y un entero.
*/
typedef struct _estructura{
	char cad[100]; /*!< Cadena de 100 caracteres*/
	int num; /*!< N*/
}estructura;


/**
* @brief determina si un numero es primo o no.
* 
* es_primo(int n) determina si n es o no primo.
* @param n numero que queremos saber si es primo.
* @return int 0 si no es primo, 1 si si lo es.
*/
int es_primo(int n){
	int i;
	for(i=2; i < sqrt(n); i++){
		if(n%i == 0){
			return 0;
		}
	}
	return 1;
}

/**
* @brief calcula todos los primos hasta un N pasado
* como parte de una estructura estructura.
* 
* calcular_primos(void *arg) calcula todos los primos hasta un N.
* @param void *args un puntero a estructura pasado como puntero a void.
* en el se pasa el numero N.
* @return void* 0 siempre a NULL.
*/
void* calcular_primos(void *arg){
	estructura *est;
	int numPrimos;
	int i;
	int N;

	est = (estructura *)arg;
	N = est->num;

	for(i=1, numPrimos=0; numPrimos < N; i++){
		if(es_primo(i) == 1){
			numPrimos++;
		}
	}
	return NULL;
}

/**
* @brief crea cien procesos, llama a calcular primos en cada uno de ellos y los espera.
* 
* main (int argc, char* argv[]) cronometra el tiempo que tarda
* en crear cien procesos que calculan los N primeros primos, donde 
* N se pasa en la entrada, y en destruirlos. Imprime dicho tiempo.
* @param argc numero de argumentos recibidos en la entrada.
* @param argv array de strings de argumentos recibidos en la entrada.
* 
* @return int que determina si el programa se ha ejecutado o no con exito.
*/
int main(int argc , char *argv[]) { 	
	pid_t pid[100];
	estructura est;
	int i;
	double tiempo;
	struct timeval ti, tf;

	if(argc < 2){
		printf("No hay suficientes argumentos de entrada. Introduce el N.\n");
		exit(EXIT_FAILURE);
	}

	est.num = atoi(argv[1]);

	gettimeofday(&ti, NULL);
	for(i = 0; i < 100; i++){
		pid[i] = fork();
		if(pid[i] == 0){
			calcular_primos(&est);
			exit(EXIT_SUCCESS);
		}
	}

	for(i = 0; i < 100; i++){
		wait(NULL); 
	} 

	gettimeofday(&tf, NULL);
	tiempo = (tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000.0;
	printf("El programa termino correctamente tardando: %lf ms\n", tiempo); 	
	exit(EXIT_SUCCESS);
}