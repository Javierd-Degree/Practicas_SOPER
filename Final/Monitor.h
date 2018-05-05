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

void monitorAntesCarrera();

void monitorDuranteCarrera(int semid, int memid, int numCaballos);

void monitorDespuesCarrera(int memCaballosId, int memApostadoresId, int numCaballos, int numApostadores);

#endif /* MONITOR_H */