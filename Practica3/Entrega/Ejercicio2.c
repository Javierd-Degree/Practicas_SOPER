/**
* @brief Primera parte del ejercicio 2 de la Practica 3.
*
* @file Ejercicio2.c
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
    if(sup < inf) return -1;
    if(sup == inf) return sup;
    /*Cuando nuestro intervalo es mayor que RAND_MAX, groups será 0, y no funcionará*/
    if(sup-inf+1 > RAND_MAX) return -1;
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
* compartida y se imprime el nombre del usuario y su id.
*/
void captura(){
	Info* inf;
	key_t key;

	/* Key de la memoria compartida */
   	key = ftok(FILEKEY, KEY);
   	if(key == -1) {
      	fprintf (stderr, "Error with key \n"); 
      	return;
	}

	/* Declaramos la memoria compartida */
   	inf = shmat (id_zone, (char*)0, 0);
	if(inf == NULL) {
      	fprintf (stderr, "Error reserve shared memory \n");
		return;
   	}

   	printf("El usuario: %s tiene id %d.\n", inf->nombre, inf->id);
}


/**
* @brief El proceso padre crea N hijos cada uno de los cuales primero duerme
* un tiempo aleatorio entre 1 y 5 segundos y luego pide por pantalla el
* nombre de un nuevo usuario y pone este usuario en la memoria compartida
* aumentando en 1 el id para que este no se repita en 2 usuarios. Tras añadir
* un usuario los procesos hijo mandan la senal SIGUSR1 al padre para que este
* ejecute la funcion de control de esta señal que imprime por pantalla el
* nombre y el id del usuario que esta en la memoria compartida. 
*
* @return int que determina si el programa se ha ejecutado o no con exito.
*/
int main(){
	int pid[NUM_HIJOS], i;
	char buffer[80];
	key_t key;
	Info* inf;


	/*Definimos la señal para el padre*/
	if(signal(SIGUSR1, captura)== SIG_ERR){
		perror("signal");
		exit(EXIT_FAILURE);
	}

	/* Key a la memoria compartida*/
   	key = ftok(FILEKEY, KEY);
   	if(key == -1) {
      	fprintf (stderr, "Error with key \n");
		return -1; 
	}

   	/* Creamos la memora compartida */
   	id_zone = shmget (key, sizeof(Info), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
   	if(id_zone == -1) {
     	fprintf (stderr, "Error with id_zone1 \n");
		return -1; 
	}

	/* Declaramos la memoria compartida para poder acceder a ella*/
   	inf = shmat (id_zone, (char*)0, 0);
	if(inf == NULL) {
      	fprintf (stderr, "Error reserve shared memory \n");
		return -1; 
   	}

   	/*Inicializamos el id de la memoria compartida a 0*/
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
			/*Pedimos por pantalla el nombre de un nuevo usuario*/
			printf("Introduce el nombre del nuevo usuario: ");
			scanf("%s", buffer);

			/* Declaramos la memoria compartida para que los hijos puedan acceder a ella */
		   	inf = shmat (id_zone, (char*)0, 0);
			if(inf == NULL) {
		      	fprintf (stderr, "Error reserve shared memory \n");
				return -1; 
		   	}

		   	/*anadimos el nombre a la memoria compartida*/
		   	strcpy(inf->nombre, buffer);
		   	inf->id ++;

		   	/*mandamos la senal SIGUSR1 al padre*/
			kill(getppid(), SIGUSR1);

			exit(EXIT_SUCCESS);
		}else{
			/*Eperamos a recibir alguna señal*/
			pause();
		} 
	}

	/*Eeramos a que acaben los procesos hijo*/
	for(i = 0; i < NUM_HIJOS; i++){
		wait(NULL);
	}

	/* Liberamos la memoria compartida */
   	shmdt ((char*)inf);
   	shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);


	exit(EXIT_SUCCESS);
}