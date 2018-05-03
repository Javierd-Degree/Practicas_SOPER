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
#include <lpthread.h>


typedef struct _mensaje{
	long type;
	char text[20];
	double apuesta;
}mensajeApuesta;

void* ventanilla();

void crearVentanillas(int N, pthread_t* h);

void esperarVentanillas(int N, pthread_t* h);


void apostador();


void gestor(int numCaballos, int numApostadores, int numVentanillas);

#endif /* GESTORAPUESTAS_H */


