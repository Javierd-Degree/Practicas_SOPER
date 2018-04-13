/**
* @brief Ejercicio 4 de la Practica 2.
*
* @file Ejercicio4.c
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

/* shm*  */
#define FILEKEY "/bin/cat"
#define KEY 1300
#define N 36 //maximo numero de elementos

int semid, id_zone;





typedef struct info{
	char array[N]; 

	int contador; // contador del numero de elementos disponibles

}Info;






int produce_item (){
	Info* inf;
	char caracter = 'a';
	int i;

	for(i = 0; i < N; i++){

		Down_Semaforo(semid, 0, SEM_UNDO);
		inf = shmat (id_zone, (char*)0, 0);
		if(inf == NULL) {
	      	fprintf (stderr, "Error reserve shared memory \n");
			return -1;
	   	}

		while(inf->contador==N); // no hace nada mientras la cola est ́e llena
		inf->contador=inf->contador+1;
		inf->array[i] = caracter;

		Up_Semaforo(semid, 0, SEM_UNDO);
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

int consume_item(){
	Info* inf;
	char caracter;
	int i;

	for(i = 0; i < N; i++){
		Down_Semaforo(semid, 0, SEM_UNDO);
		/* we declared to zone to share */
	   	inf = shmat (id_zone, (char*)0, 0);
		if(inf == NULL) {
	      	fprintf (stderr, "Error reserve shared memory \n");
			return -1;
	   	}

		while(inf->contador == 0); // no hace nada mientras la cola este vacia
		inf->contador = inf->contador -1;

		caracter = inf->array[i];

		Up_Semaforo(semid, 0, SEM_UNDO);

	   	printf("%c ", caracter);

	   	if(caracter == '9'){
	   		return 0;
	   	}
	}
	return 0;
}












/**
* @brief El proceso padre crea un proceso hijo, que
* imprime 10 veces "Soy <PID> y estoy trabajando", esperando 
* un segundo entre cada vez. Una vez el proceso hijo ha impreso
* el texto las 10 veces, manda la señal SIGUSR1 al padre, que sale
* del pause(), y crea otro hijo, de forma que es este nuevo hijo
* el que manda una senal SIGUSR2 al hijo anterior para que este se
* termine a el mismo.
* Asi, el padre acaba creando NUM_HIJOS procesos hijos, el mismo 
* mata al ultimo de los hijos, y se asegura de esperar a todos.
*
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

	/* Key to shared memory */
   	key = ftok(FILEKEY, KEY);
   	if(key == -1) {
      	fprintf (stderr, "Error with key \n");
		return -1; 
	}

   	/* we create the shared memory */
   	id_zone = shmget (key, sizeof(char[N]), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
   	if(id_zone == -1) {
     	fprintf (stderr, "Error with id_zone1 \n");
		return -1; 
	}

	/* we declared to zone to share */
   	inf = shmat (id_zone, (char*)0, 0);
	if(inf == NULL) {
      	fprintf (stderr, "Error reserve shared memory \n");
		return -1; 
   	}

   	inf->contador = 0;

	pid = fork();
	if(pid < 0){
		printf("Error al crear el proceso.\n");
		exit(EXIT_FAILURE);
	}else if(pid == 0){
		while(1){
			ret = consume_item();
			if(ret == -1){
				free(array);
				exit(EXIT_FAILURE);
			}else if(ret == 0){
				free(array);
				exit(EXIT_SUCCESS);
			}
		}
	}else{
		while(1){
			ret = produce_item ();	
			if(ret == 0){
				/* Free the shared memory */
		   		wait(NULL);
			   	shmdt ((char*)inf);
			   	shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);

			   	int Borrar_Semaforo(int semid);
			   	free(array);
			   	exit(EXIT_SUCCESS);
			}else if(ret == -1){
				/* Free the shared memory */
				wait(NULL);
			   	shmdt ((char*)inf);
			   	shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);

			   	int Borrar_Semaforo(int semid);
			   	free(array);
				exit(EXIT_FAILURE);
			}		
		}
	} 
}