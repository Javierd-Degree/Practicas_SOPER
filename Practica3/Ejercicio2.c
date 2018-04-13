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
* Funcion a la que le pasas 2 numeros y devuelve un numero aleatorio entre ambos.
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
* la senal SIGUSR1. De esta forma, nos aseguramos que el
* proceso salga correctamente del pause() y continue su
* ejecucion.
*/
void captura(){
	Info* inf;
	key_t key;

	/* Key to shared memory */
   	key = ftok(FILEKEY, KEY);
   	if(key == -1) {
      	fprintf (stderr, "Error with key \n"); 
      	return;
	}

	/* we declared to zone to share */
   	inf = shmat (id_zone, (char*)0, 0);
	if(inf == NULL) {
      	fprintf (stderr, "Error reserve shared memory \n");
		return;
   	}

   	printf("The user %s has the id %d.\n", inf->nombre, inf->id);
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
	int pid[NUM_HIJOS], i;
	char buffer[80];
	key_t key;
	Info* inf;
	int ret;


	/*Definimos la señal para el padre*/
	if(signal(SIGUSR1, captura)== SIG_ERR){
		perror("signal");
		exit(EXIT_FAILURE);
	}

	/* Key to shared memory */
   	key = ftok(FILEKEY, KEY);
   	if(key == -1) {
      	fprintf (stderr, "Error with key \n");
		return -1; 
	}

   	/* we create the shared memory */
   	id_zone = shmget (key, sizeof(Info), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
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

   	/*we initialise the id to 0 at the begining*/
   	inf->id = 0;


	for(i = 0; i < NUM_HIJOS; i++){
		pid[i] = fork();
		if(pid < 0){
			exit(EXIT_FAILURE);
			printf("Error al crear el proceso.\n");
		}else if(pid[i] == 0){
			sleep(aleat_num(1, 5)); 
			printf("Introduce el nombre del nuevo usuario: ");
			scanf("%s", buffer);

			/* we declared to zone to share */
		   	inf = shmat (id_zone, (char*)0, 0);
			if(inf == NULL) {
		      	fprintf (stderr, "Error reserve shared memory \n");
				return -1; 
		   	}

		   	strcpy(inf->nombre, buffer);
		   	inf->id ++;

			kill(getppid(), SIGUSR1);

			free(array);

			exit(EXIT_SUCCESS);
		}else{
			pause();
		} 
	}

	for(i = 0; i < NUM_HIJOS; i++){
		wait(NULL);
	}

	/* Free the shared memory */
   	shmdt ((char*)inf);
   	shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);

   	free(array);

	exit(EXIT_SUCCESS);
}