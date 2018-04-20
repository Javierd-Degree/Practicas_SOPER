/**
* @brief Ejercicio 3 de la Practica 3.
*
* @file Ejercicio3.c
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h> 
#include <errno.h>
#include "LibreriaSemaforos.h"
/*Id del array de semaforos*/
#define SEMKEY 75798

#define N_SEMAFOROS 1

/* shm  */
#define FILEKEY "/bin/cat"
#define KEY 1300
#define N 36 //maximo numero de elementos

int semid, id_zone;





typedef struct info{
	char array[N]; 

	int contador; // contador del numero de elementos disponibles

}Info;





/**
* @brief Funcion que produce los elementos, que en este caso son caracteres.
* crea los caracteres de la 'a' a la 'z' y los numeros del 0 al 9 y
* los almacena en la memoria compartida.
*
* @return int que indica si la funcion se ha ejecutado correctamente.
*/
int produce_item (){
	Info* inf;
	/*Inicializamos el caracter a 'a'*/
	char caracter = 'a';
	int i;

	for(i = 0; i < N; i++){
		/*Bajamos el semaforo*/
		Down_Semaforo(semid, 0, SEM_UNDO);
		/*Declaramos la memoria compartida*/
		inf = shmat (id_zone, (char*)0, 0);
		if(inf == NULL) {
	      	fprintf (stderr, "Error reserve shared memory \n");
			return -1;
	   	}

		while(inf->contador==N); // no hace nada mientras la cola est ́e llena
		/*Incrementamos el contador*/
		inf->contador=inf->contador+1;
		/*Añadimos el caracter a la memoria compartida*/
		inf->array[i] = caracter;
		/*Subimos el semaforo*/
		Up_Semaforo(semid, 0, SEM_UNDO);
		/*incrementamos en 1 el caracter*/
		if(caracter == 'z'){
			caracter = '0';
		}else if(caracter == '9'){
			return 0;
		}else{
			caracter ++;
		}
	}
	return 0;
}

/**
* @brief Funcion que consume los elementos. En este caso
* consumirlos consiste en ir leyendolos de la memoria
* compartida e irlos imprimiendo por pantalla. 
*
* @return int que indica si la funcion se ha ejecutado con exito.
*/
int consume_item(){
	Info* inf;
	char caracter;
	int i;

	for(i = 0; i < N; i++){
		/*Bajamos el semaforo*/
		Down_Semaforo(semid, 0, SEM_UNDO);
		/* Declaramos la memoria compartida */
	   	inf = shmat (id_zone, (char*)0, 0);
		if(inf == NULL) {
	      	fprintf (stderr, "Error reserve shared memory \n");
			return -1;
	   	}

		while(inf->contador == 0); // no hace nada mientras la cola este vacia
		/*Disminuimos el contador*/
		inf->contador = inf->contador -1;
		/*Cogemos el caracter de la memoria compartida*/
		caracter = inf->array[i];
		/*Subimos el semaforo*/
		Up_Semaforo(semid, 0, SEM_UNDO);
		/*Imprimimos por pantalla el caracter*/
	   	printf("%c ", caracter);
	   	/*Comprobamos que si el caracter es 9 se acaba el proceso*/
	   	if(caracter == '9'){
	   		return 0;
	   	}
	}
	return 0;
}












/**
* @brief El proceso padre inicializa una zona de memoria compartida y un
* semaforo paracontrolar el acceso a esta, tras esto crea un proceso hijo,
* que consume los elementos creados por el productor con la funcion
* consume_item() hasta que esta devuelve 0 en cuyo caso termina su ejecucion
* (pues se habra llegado ya al ultimo). Mientras tanto el padre se va
* encargando de producir los objetos e introducirlos en la memoria compartida
* con la funcion produce_item, hasta que esta devuelve 0 indicando que se ha
* creado ya el ultimo elemento, tras lo cual espera a que acabe la ejecucion
* del hijo, libera la memoria compartida y los semaforos y termina su ejecucion.

* @return int que determina si el programa se ha ejecutado o no con exito.
*/
int main(){
	int i, pid;
	key_t key;
	Info* inf;
	int ret;
	unsigned short *array;

	/*Inicializamos el array de semaforos a 1*/
	array = (unsigned short *)malloc(sizeof(unsigned short)*N_SEMAFOROS);
	if(array == NULL){
		printf("Error al reservar memoria\n");
	}

	for(i = 0; i < N_SEMAFOROS; i++){
		array[i] = 1;
	}

	/*Creamos e inicializamos los semaforos a 1*/
	ret = Crear_Semaforo(SEMKEY, N_SEMAFOROS, &semid);
	if(ret == ERROR){
		perror("Error al crear los semaforos");
		exit(EXIT_FAILURE);
	}

	ret = Inicializar_Semaforo(semid, array);
	if(ret == ERROR){
		perror("Error al inicializar los semaforos");
		exit(EXIT_FAILURE);
	}

	/* Key a la memoria compartida*/
   	key = ftok(FILEKEY, KEY);
   	if(key == -1) {
      	fprintf (stderr, "Error with key \n");
		return -1; 
	}

   	/* Creamos la zona de memoria compartida */
   	id_zone = shmget (key, sizeof(char[N]), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
   	if(id_zone == -1) {
     	fprintf (stderr, "Error with id_zone1 \n");
		return -1; 
	}

	/* Declaramos la memoria compartida*/
   	inf = shmat (id_zone, (char*)0, 0);
	if(inf == NULL) {
      	fprintf (stderr, "Error reserve shared memory \n");
		return -1; 
   	}

   	/*Inicializamos el contador de la memoria compartida a 0*/
   	inf->contador = 0;

   	/*Creamos el hijo que sera el consumidor*/
	pid = fork();
	if(pid < 0){
		printf("Error al crear el proceso.\n");
		exit(EXIT_FAILURE);
	}else if(pid == 0){
		/*Consumimos los elementos*/
		ret = consume_item();
		if(ret == -1){
			free(array);
			exit(EXIT_FAILURE);
		}else if(ret == 0){
			free(array);
			exit(EXIT_SUCCESS);
		}
	}else{
		/*Producimos los elementos*/
		ret = produce_item ();	
		if(ret == 0){
			/* Liberamos la memoria compartida */
	   		wait(NULL);
		   	shmdt ((char*)inf);
		   	shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
		   	/*Liberamos los semaforos*/
		   	int Borrar_Semaforo(int semid);
		   	free(array);
		   	exit(EXIT_SUCCESS);
		}else if(ret == -1){
			/* Liberamos la memoria compartida */
			wait(NULL);
		   	shmdt ((char*)inf);
		   	shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
		   	/*Liberamos los semaforos*/
		   	int Borrar_Semaforo(int semid);
		   	free(array);
			exit(EXIT_FAILURE);
		}	
	} 
}