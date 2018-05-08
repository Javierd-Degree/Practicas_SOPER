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
#include "Monitor.h"
#include "Semaforo.h"
#include "GestorApuestas.h"

#define KEY 11000

int main(){
	recursosCaballo resCaballo;
	recursosGestor resGestor;
	int numCaballos;
	int longCarrera;
	int numApostadores;
	int numVentanillas;
	int maxApuesta;
	int res;
	int pidMonitor;
	int pidGestor;
	int pidApostador; 	
	int semid;
	unsigned short semvalor;

	printf("Introduce la longitud de carrera: ");
	scanf("%d", &longCarrera);

	do{
		printf("Introduce el numero de caballos: ");
		scanf("%d", &numCaballos);
	}while(numCaballos > 10);

	do{
		printf("Introduce el numero de apostadores: ");
		scanf("%d", &numApostadores);
	}while(numApostadores > 100);

	printf("Introduce el dinero de cada apostador: ");
	scanf("%d", &maxApuesta);

	printf("Introduce el número de ventanillas: ");
	scanf("%d", &numVentanillas);

	/*Creamos e inicializamos el semáforo que controlará la entrada a la memoria compartida*/
	res = Crear_Semaforo((key_t)KEY, 1,  &semid);
	if(res == -1){
		printf("Error al crear el array de semáforos\n");
		return -1;
	}

	semvalor = 0;

	res = Inicializar_Semaforo(semid, &semvalor);
	if(res == -1){
		printf("Error al inicializar el array de semáforos\n");
		return -1;
	}

	res = inicializaRecursosCaballo(&resCaballo, numCaballos);
	if(res == -1){
		printf("Error en inicializaRecursosCaballo\n");
	}

	res = inicializaRecursosGestor(&resGestor);
	if(res == -1){
		printf("Error en inicializaRecursosGestor\n");
	}

	pidMonitor = fork();
	if(pidMonitor == -1){
		printf("Error al hacer fork.\n");
	}else if(pidMonitor == 0){
		monitorAntesCarrera(numCaballos, resGestor.memid, resGestor.semid);
		res = Up_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo");
		}
		monitorDuranteCarrera(resCaballo.semid, resCaballo.memid, numCaballos, longCarrera);
		printf("El monitor de carrera ha acabado.\n");
		exit(EXIT_SUCCESS);
	}

	pidGestor = fork();
	if(pidGestor == -1){
		printf("Error al hacer fork.\n");
	}else if(pidGestor == 0){
		gestor(numCaballos, numApostadores, numVentanillas, &resGestor);
		exit(EXIT_SUCCESS);
	}

	pidApostador = fork();
	if(pidApostador == -1){
		printf("Error al hacer fork.\n");
	}else if(pidApostador == 0){
		apostador(numCaballos, numApostadores, maxApuesta, &resGestor);
		exit(EXIT_SUCCESS);
	}


	printf("Gestor %d\n", pidGestor);
	printf("Apostador %d\n", pidApostador);
	printf("Monitor %d\n", pidMonitor);

	res = Down_Semaforo(semid, 0, SEM_UNDO);
	if(res == -1){
		printf("Error al bajar el semáforo");
	}

	/*El gestor y el apostador tienen que acabar obligatoriamente.*/
	kill(pidGestor, SIGUSR1);
	kill(pidApostador, SIGKILL);

	carrera(numCaballos, longCarrera, resCaballo.semid, resCaballo.memid);
	printf("CARRERA ACABADA\n");

	waitpid(pidMonitor);

	res = Borrar_Semaforo(semid);
	if(res == -1){
		printf("Error al borrar el array de semáforos\n");
		return -1;
	}

	res = liberarRecursosGestor(&resGestor);
	if(res == -1){
		printf("Error en liberarRecursosGestor\n");
	}

	res = liberarRecursosCaballo(&resCaballo);
	if(res == -1){
		printf("Error en liberarRecursosCaballo\n");
	}
	exit(EXIT_SUCCESS);
}