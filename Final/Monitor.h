/**
* @brief libreria del monitor del Proyecto Final.
*
* @file Monitor.h
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/

#ifndef MONITOR_H
#define MONITOR_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <syslog.h>
#include "Semaforo.h"

#define NEXT_SIGNAL SIGUSR2


typedef struct _compartida{
	double cotizaciones[10];
	double pagar[100];
	double apuestas[10];
	double totalApostado;
	double cantidadApostada[100];
	int apostadorCaballo[100];
}memCompartida;

void monitorAntesCarrera(int numCaballos, int memid, int semid);

void monitorDuranteCarrera(int semid, int memid, int numCaballos, int longCarrera);

void monitorDespuesCarrera(int memCaballosId, int memApostadoresId, int numCaballos, int numApostadores, int dineroApostador, int longCarrera, int semid);

void monitorImprimeReport(char* string, int semid);

#endif /* MONITOR_H */