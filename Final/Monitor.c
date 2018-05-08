#include "Monitor.h"

void monitor(int semCaballos, int memCaballos, int numCaballos){

}

void monitorAntesCarrera(int numCaballos, int memid, int semid){
	int i;
	int j;
	int res;
	memCompartida* mem;

	for(i = 30; i > 0; i--){
		printf("Quedan %d segundos para que la carrera comience.\n", i);
		printf("La cotización de los caballos es: \n");

		res = Down_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semaforo de la memoria compartida.\n");
		}

		mem = (memCompartida*)shmat(memid, (char)0, 0);
		if(mem == NULL){
			printf("Error en la memoria compartida.\n");
		}

		for(j = 0; j < numCaballos; j++){
			printf("Caballo %d: %lf\n", j, mem->cotizaciones[j]);
		}

		res = Up_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semaforo de la memoria compartida.\n");
		}
		usleep(1000000);
	}
	shmdt(mem);
	return;
}

void monitorDuranteCarrera(int semid, int memid, int numCaballos){
	/*TODO Quiza sea mejor hacerlo mediante semaforos/ senales para
	que se detenga despues de cada tirada.*/
	int i;
	int res;
	int *memCaballos;
	int flag = 0;
	int anterior[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	/*Tenemos que imprimir lo que hacen los caballos
	en cada momento.*/
	memCaballos = (int*)shmat(memid, (char*)0, 0);
	if(memCaballos == NULL) {
		printf ("Error reserve shared memory \n");
		exit(EXIT_FAILURE);
	}

	while(1){
		/*Rendezvous.*/
		res = Up_Semaforo(semid, 1, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo del rendezvous.");
		}
		res = Down_Semaforo(semid, 2, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semáforo del rendezvous.");
		}
		


		res = Down_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semáforo");
		}
		for(i = 0; i < numCaballos; i++){
			/*Nos aseguramos de que la posicion haya cambiado*/
			if(anterior[i] != memCaballos[i]){
				anterior[i] = memCaballos[i];
				printf("Caballo %d en posicion %d tras haber tirado %d\n", i, memCaballos[i], memCaballos[numCaballos + i]);
				if(i == numCaballos - 1){
					printf("\n");
				}
			}
		}
		res = Up_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo");
		}

		if(flag){
			break;
		}

		/*Rendezvous 2.*/
		res = Up_Semaforo(semid, 3, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo del rendezvous 2.");
		}
		res = Down_Semaforo(semid, 4, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semáforo del rendezvous 2.");
		}
	}

	shmdt(memCaballos);
}

void monitorDespuesCarrera(int memCaballosId, int memApostadoresId, int numCaballos, int numApostadores){
	int i;
	int *memCaballos;

	memCaballos = (int*)shmat(memCaballosId, (char*)0, 0);
	if(memCaballos == NULL) {
		printf ("Error reserve shared memory \n");
		exit(EXIT_FAILURE);
	}

	/*Imprimimos la informacion de los caballos*/
	printf("La carrera ya ha acabado y las posiciones son:\n");
	/*Como ya ha acabado la carrera, no es necesario usar el 
	semaforo de los caballos.*/
	for(i = 0; i < numCaballos; i++){
		printf("\t -Caballo %d, en posicion %d", i+1, memCaballos[i]);
	}

	shmdt(memCaballos);


	/*Imprimimos la informacion de los apostadores.*/

	return;
}