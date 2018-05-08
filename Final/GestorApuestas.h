#ifndef GESTORAPUESTAS_H
#define GESTORAPUESTAS_H

#include "Caballos.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include "Utils.h"
#include "Monitor.h"


typedef struct _mensajeGestor{
	long type;
	char text[20];
	double apuesta;
}mensajeApuesta;

typedef struct _recursosGestor{
	int memid;
	int semid;
	int mensaje_id;
	int numVentanilla;
}recursosGestor;


#define SEMKEYGESTOR 17000
#define FILE_MEM_COMP_GESTOR_KEY "/bin/ls"
#define MEM_COMP_GESTOR_KEY 12000
#define FILE_VENTANILLA_KEY "/bin/ls"
#define VENTANILLA_KEY 33

int inicializaRecursosGestor(recursosGestor* recs);

int liberarRecursosGestor(recursosGestor* recs);

void* ventanilla(void* recs);

void crearVentanillas(int N, pthread_t* h, recursosGestor* recs);

void esperarVentanillas(int N, pthread_t* h);


void apostador(int numCaballos, int numApostadores, int maxApuesta, recursosGestor* recs);

void salida();

void gestor(int numCaballos, int numApostadores, int numVentanillas, recursosGestor* recs);

#endif /* GESTORAPUESTAS_H */


