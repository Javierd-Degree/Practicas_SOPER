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

int main(){
	recursosCaballo resCaballo;
	int numCaballos;
	int longCarrera;
	int res;
	int pidMonitor;

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

	pidMonitor = fork();
	if(pidMonitor == -1){
		printf("Error al hacer fork.\n");
	}else if(pidMonitor == 0){
		monitorDuranteCarrera(resCaballo.semid, resCaballo.memid, numCaballos);
	}

	carrera(numCaballos, longCarrera, resCaballo.semid, resCaballo.memid);
	kill(pidMonitor, SIGKILL);


	res = liberarRecursosCaballo(&resCaballo);
	if(res == -1){
		printf("Error en liberarRecursosCaballo\n");
	}
	exit(EXIT_SUCCESS);
}