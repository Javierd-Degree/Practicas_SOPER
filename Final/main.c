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