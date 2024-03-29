/**
* @brief Caballos de la practica final.
*
* @file Caballos.h
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/

#ifndef CABALLOS_H
#define CABALLOS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <syslog.h>
#include "Semaforo.h"

#define MAX_CABALLOS 10

#define MEDIO 0
#define PRIMERO 1
#define ULTIMO 2

#define CABALLO_SEMKEY 75798

#define FILE_CABALLO_KEY "/bin/ls"
#define CABALLO_KEY 1500

#define FILE_CABALLO_MEM_KEY "/bin/cat"
#define CABALLO_MEM_KEY 1600

typedef struct _mensaje{
	long type;
	char text[100];
}mensajeCaballo;

typedef struct _recursosCaballo{
	int semid;
	int memid;
}recursosCaballo;

void carrera(int numCaballos, int longCarrera, int semid, int memid);

int inicializaRecursosCaballo(recursosCaballo *r, int numCaballos);

int liberarRecursosCaballo(recursosCaballo *r);

int caballoAvanza(int modo);

int carreraAcabada(int *pids, int numCaballos);

int posicionCaballo(int num, int *lista, int numCaballos);

void caballo(int numero, int pipe[2], int pipe2[2], int lonCarrera);

#endif /* CABALLOS_H */
