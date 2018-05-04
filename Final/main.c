#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "Caballos.h"
#include "Semaforo.h"

void carrera(int numCaballos, int longCarrera, int semid, int memid){
	int i;
	int res;
	int pids[MAX_CABALLOS];
	int *memCaballos;
	int pipesCaballos[20][2];
	int ganador = 0;

	char temp[124];

	/*Guardamos en primer lugar todas las posiciones, en segundo 
	lugar todas las tiradas.*/
	memCaballos = (int*)shmat(memid, (char*)0, 0);
	if(memCaballos == NULL) {
		printf ("Error reserve shared memory \n");
		exit(EXIT_FAILURE);
	}

	/*Inicializamos todos los pipes.*/
	for(i = 0; i < numCaballos; i++){
		if(pipe(pipesCaballos[i]) == -1 || pipe(pipesCaballos[numCaballos + i]) == -1){
			printf("Error al inicializar los pipes.\n");
			exit(EXIT_FAILURE);
		}
	}

	/*Creamos todos los caballos.*/
	for(i = 0; i < numCaballos; i++){
		pids[i] = fork();
		if(pids[i] == -1){
			printf("Error al hacer fork.\n");
			exit(EXIT_FAILURE);
		}else if(pids[i] == 0){
			/* CAMBIAR FUNCION.*/
			caballo(i+1, pipesCaballos[i], pipesCaballos[numCaballos + i], longCarrera);
			exit(EXIT_SUCCESS);
		}else{
			printf("ID Caballo %d: %d\n", i+1, pids[i]);
			close(pipesCaballos[i][0]);
			close(pipesCaballos[numCaballos + i][1]);
		}
	}

	/*Empieza la carrera*/
	while(!ganador){
		/*Hacemos las tiradas de todos los caballos.*/
		res = Down_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semáforo");
		}

		for(i = 0; i < numCaballos; i++){
			/*Le mandamos a cada caballo la posicion en
			la que esta, para que haga su tirada.*/
			sprintf(temp, "%d", posicionCaballo(i, memCaballos, numCaballos));
			write(pipesCaballos[i][1], temp, strlen(temp)+1);
		}

		res = Up_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo");
		}

		/*Esperamos a todos los caballos y actualizamos
		sus posiciones*/
		res = Down_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semáforo");
		}
		for(i = 0; i < numCaballos; i++){
			read(pipesCaballos[numCaballos + i][0], temp, 124);
			/*Guardamos la tirada y actualizamos la posicion.*/
			memCaballos[numCaballos + i] = atoi(temp);
			memCaballos[i] += atoi(temp);

			if(memCaballos[i] >= longCarrera){
				ganador = 1;
			}
		}
		res = Up_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo");
		}
	}

	/*Finalizamos todos los caballos*/
	for(i = 0; i < numCaballos; i++){
		kill(pids[i], SIGKILL);
		wait(NULL);
	}

	/*Imprimimos los resultados de los caballos*/
	printf("La carrera ya ha acabado y las posiciones son:\n");
	res = Down_Semaforo(semid, 0, SEM_UNDO);
	if(res == -1){
		printf("Error al bajar el semáforo");
	}
	for(i = 0; i < numCaballos; i++){
		printf("Caballo %d, en posicion %d, con ultima tirada %d.\n", i+1, memCaballos[i], memCaballos[numCaballos + i]);
		/*Guardamos la tirada y actualizamos la posicion.*/
		memCaballos[numCaballos + i] = atoi(temp);
		memCaballos[i] += atoi(temp);
		if(memCaballos[i] >= longCarrera){
			ganador = 1;
		}
	}
	res = Up_Semaforo(semid, 0, SEM_UNDO);
	if(res == -1){
		printf("Error al subir el semáforo");
	}

	shmdt(memCaballos);
}

int main(){
	recursosCaballo resCaballo;
	int numCaballos;
	int longCarrera;
	int res;

	printf("Introduce la longitud de carrera: ");
	scanf("%d", &longCarrera);

	do{
		printf("Introduce el numero de caballos: ");
		scanf("%d", &numCaballos);
	}while(numCaballos > 10);

	res = inicializaRecursosCaballo(&resCaballo, numCaballos);
	if(res == -1){
		printf("Error en inicializaRecursosCaballo\n");
	}

	carrera(numCaballos, longCarrera, resCaballo.semid, resCaballo.memid);

	res = liberarRecursosCaballo(&resCaballo);
	if(res == -1){
		printf("Error en liberarRecursosCaballo\n");
	}
	exit(EXIT_SUCCESS);
}