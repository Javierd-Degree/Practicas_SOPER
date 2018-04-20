/**
* @brief Segunda parte del ejercicio 2 de la Practica 3.
*
* @file Ejercicio2_solved.c
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

#define NUM_HIJOS 5 /*!< Numero de hijos que crea el proceso padre*/

typedef struct info{
     char nombre[80];
     int id;
}Info;

int semid, id_zone;

/**
* @brief Funcion a la que le pasas 2 numeros y devuelve un numero aleatorio entre ambos.
*
* @param inf Int con el numero mas bajo que se quiere.
* @param sup Int con el numero mas alto que se quiere.
* @return int con un número aleatorio entre inf y sup. 
*/
int aleat_num(int inf, int sup){
    if(sup < inf) return ERROR;
    if(sup == inf) return sup;
    /*Cuando nuestro intervalo es mayor que RAND_MAX, groups será 0, y no funcionará*/
    if(sup-inf+1 > RAND_MAX) return ERROR;
    int random;
    int range = sup-inf+1;
    int groups = RAND_MAX/range;
    int new_lim = groups * range;

    do{
    	random = rand();
    }while(random >= new_lim);

    return (random/groups) + inf;
}

/**
* @brief Funcion que ejecuta el proceso padre tras recibir
* la senal SIGUSR1. En esta funcion se lee la region de zona
* compartida y se imprime el nombre del usuario y su id. En
* este caso para solucionar el problema de que varios procesos
* accedan a la memoria compartida a la vez, se realiza un down
* de un semaforo justo antes de acceder a la memoria compartida
* y un up al acabar de acceder a esta.
*/
void captura(){
	Info* inf;
	key_t key;

	/*Key a la memoria compartida */
   	key = ftok(FILEKEY, KEY);
   	if(key == -1) {
      	fprintf (stderr, "Error en la key \n"); 
      	return;
	}
	/*Bajamos el semaforo*/
	Down_Semaforo(semid, 0, SEM_UNDO);
	/* Declaramos la zona compartida */
   	inf = shmat (id_zone, (char*)0, 0);
	if(inf == NULL) {
      	fprintf (stderr, "Error al reservar memoria compartida \n");
		return;
   	}

   	/*Imprimimos por pantalla los datos de la memoria compartida*/ 
   	printf("El usuario: %s tiene id %d.\n", inf->nombre, inf->id);
   	/*Subimos el semaforo*/
   	Up_Semaforo(semid, 0, SEM_UNDO);
}


/**
* @brief El proceso padre crea N hijos cada uno de los cuales primero duerme
* un tiempo aleatorio entre 1 y 5 segundos y luego pide por pantalla el
* nombre de un nuevo usuario y pone este usuario en la memoria compartida
* aumentando en 1 el id para que este no se repita en 2 usuarios. Tras añadir
* un usuario los procesos hijo mandan la señal SIGUSR1 al padre para que este
* ejecute la funcion de control de esta senal que imprime por pantalla el
* nombre y el id del usuario que esta en la memoria compartida. Para la
* sincronizacion en este caso, para evitar que varios procesos entren a la
* vez a la memoria compartida se realiza mediante semaforos haciendo un down
* antes de acceder a esta y un up al terminar de acceder.
*
* @return int que determina si el programa se ha ejecutado o no con exito.
*/
int main(){
	int pid[NUM_HIJOS], i;
	char buffer[80];
	key_t key;
	Info* inf;
	int ret;
	unsigned short *array;

	/*Inicializamos el array de para crear los semaforos a 1*/
	array = (unsigned short *)malloc(sizeof(unsigned short)*N_SEMAFOROS);
	if(array == NULL){
		printf("Error al reservar memoria\n");
	}

	for(i = 0; i < N_SEMAFOROS; i++){
		array[i] = 1;
	}

	/*Definimos la señal para el padre*/
	if(signal(SIGUSR1, captura)== SIG_ERR){
		perror("signal");
		exit(EXIT_FAILURE);
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

	/* Key a la memoria compartida */
   	key = ftok(FILEKEY, KEY);
   	if(key == -1) {
      	fprintf (stderr, "Error with key \n");
		return -1; 
	}

   	/*Creamos la zona de memoria compartida*/
   	id_zone = shmget (key, sizeof(Info), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
   	if(id_zone == -1) {
     	fprintf (stderr, "Error with id_zone1 \n");
		return -1; 
	}

	/*Declaramos la memoria compartida*/
   	inf = shmat (id_zone, (char*)0, 0);
	if(inf == NULL) {
      	fprintf (stderr, "Error reserve shared memory \n");
		return -1; 
   	}

   	/*Inicializamos a 0 el id del usuario de la memoria compartida*/
   	inf->id = 0;


	for(i = 0; i < NUM_HIJOS; i++){
		/*Creamos los hijos*/
		pid[i] = fork();
		if(pid < 0){
			exit(EXIT_FAILURE);
			printf("Error al crear el proceso.\n");
		}else if(pid[i] == 0){
			/*El proceso duerme entre 1 y 5 segundos*/
			sleep(aleat_num(1, 5));
			/*Pedimos por pantalla el nombre del nuevo usuario*/ 
			printf("Introduce el nombre del nuevo usuario: ");
			scanf("%s", buffer);
			/*Bajamos el semaforo*/
			Down_Semaforo(semid, 0, SEM_UNDO);
			/* Declaramos la memoria compartida */
		   	inf = shmat (id_zone, (char*)0, 0);
			if(inf == NULL) {
		      	fprintf (stderr, "Error reserve shared memory \n");
				return -1; 
		   	}
		   	/*Introducimos el nombre en la memoria compartida*/
		   	strcpy(inf->nombre, buffer);
		   	inf->id ++;
		   	/*Subimos el semaforo*/
		   	Up_Semaforo(semid, 0, SEM_UNDO);
		   	/*Enviamos la senal SIGUSR1 al padre*/
			kill(getppid(), SIGUSR1);

			free(array);

			exit(EXIT_SUCCESS);
		}else{
			/*Esperamos a recibir una senal*/
			pause();
		} 
	}

	/*Esperamos a que terminen los procesos hijo*/
	for(i = 0; i < NUM_HIJOS; i++){
		wait(NULL);
	}

	/* Liberamos la memoria compartida*/
   	shmdt ((char*)inf);
   	shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
   	/*Liberamos el semaforo*/
	int Borrar_Semaforo(int semid);

	free(array);

	exit(EXIT_SUCCESS);
}