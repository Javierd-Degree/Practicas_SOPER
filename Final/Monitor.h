#ifndef MONITOR_H
#define MONITOR_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "Semaforo.h"

#define NEXT_SIGNAL SIGUSR2


typedef struct _compartida{
	double cotizaciones[10];
	double pagar[100];
	double apuestas[10];
	double totalApostado;
}memCompartida;

void monitorAntesCarrera(int numCaballos, int memid, int semid);

void monitorDuranteCarrera(int semid, int memid, int numCaballos, int longCarrera);

void monitorDespuesCarrera(int memCaballosId, int memApostadoresId, int numCaballos, int numApostadores);

void monitorImprimeReport(char* string, int semid);

#endif /* MONITOR_H */