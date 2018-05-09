#include "Monitor.h"

void monitor(int semCaballos, int memCaballos, int numCaballos){

}

void monitorAntesCarrera(int numCaballos, int memid, int semid){
	int i;
	int j;
	int res;
	memCompartida* mem;

	mem = (memCompartida*)shmat(memid, (char)0, 0);
	if(mem == NULL){
		printf("Error en la memoria compartida.\n");
	}

	for(i = 30; i > 0; i--){
		printf("Quedan %d segundos para que la carrera comience.\n", i);
		printf("La cotización de los caballos es: \n");

		res = Down_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semaforo de la memoria compartida.\n");
		}

		for(j = 0; j < numCaballos; j++){
			printf("\tCaballo %d: %lf\n", j, mem->cotizaciones[j]);
		}

		res = Up_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semaforo de la memoria compartida.\n");
		}
		/*TODO Cambiar*/
		usleep(100000);
	}

	shmdt(mem);
	return;
}

void monitorDuranteCarrera(int semid, int memid, int numCaballos, int longCarrera){
	int i;
	int res;
	int *memCaballos;
	int flag = 0;
	int ganador = 0;

	/*Tenemos que imprimir lo que hacen los caballos
	en cada momento.*/
	memCaballos = (int*)shmat(memid, (char*)0, 0);
	if(memCaballos == NULL) {
		printf ("Error reserve shared memory \n");
		exit(EXIT_FAILURE);
	}

	while(!ganador){
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
			printf("Caballo %d en posicion %d tras haber tirado %d\n", i, memCaballos[i], memCaballos[numCaballos + i]);
			if(memCaballos[i] >= longCarrera){
				ganador = 1;
			}
			if(i == numCaballos - 1){
				printf("\n");
			}
		}
		res = Up_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo");
		}

		if(ganador) break;

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
	return;
}

void monitorDespuesCarrera(int memCaballosId, int memApostadoresId, int numCaballos, int numApostadores, int dineroApostador, int longCarrera, int semid){
	int i;
	int j = 0;
	int *memCaballos;
	memCompartida* mem;
	int ganadores[10];
	double beneficio;
	char infoApostador[100];
	char resultado [100];

	for(i=0; i<10; i++){
		ganadores[i] = -1;
	}

	memCaballos = (int*)shmat(memCaballosId, (char*)0, 0);
	if(memCaballos == NULL) {
		printf ("Error reserve shared memory \n");
		exit(EXIT_FAILURE);
	}

	/*Imprimimos la informacion de los caballos*/
	sprintf(resultado, "La carrera ya ha acabado y las posiciones son:\n");
	printf("%s", resultado);
	monitorImprimeReport(resultado, semid);
	/*Como ya ha acabado la carrera, no es necesario usar el 
	semaforo de los caballos.*/
	for(i = 0; i < numCaballos; i++){
		if(memCaballos[i] >= longCarrera){
			ganadores[j] = i;
			j++;
		}
		sprintf(resultado, "\t -Caballo %d, en posicion %d\n", i, memCaballos[i]);
		printf("%s", resultado);
		monitorImprimeReport(resultado, semid);
	}

	for(i = 0; ganadores[i] != -1; i++){
		printf("\t -Caballo ganador %d, en posicion %d\n", ganadores[i], memCaballos[i]);
	}



	shmdt(memCaballos);

	/*Imprimimos la informacion de los apostadores.*/

	mem = (memCompartida*)shmat(memApostadoresId, (char)0, 0);
	if(mem == NULL){
		printf("Error en el shmat de losapostadores.\n");
		exit(EXIT_FAILURE);
	}

	for(i = 1; i<=numApostadores; i++){
		for(j=0; ganadores[j] != -1; j++){
			if(mem->apostadorCaballo[i] == ganadores[j]){
				beneficio = mem->pagar[i] - mem->cantidadApostada[i];
				break;
			}else{
				beneficio = 0 - mem->cantidadApostada[i];
			}
		}
		sprintf(infoApostador, "Apostador-%d ha apostado %lf y ha obtenido %lf beneficios, por lo que le queda %lf.\n", i, mem->cantidadApostada[i], beneficio, dineroApostador + beneficio);
		monitorImprimeReport(infoApostador, semid);
	}

	shmdt(mem);
	

	return;
}

void monitorImprimeReport(char* string, int semid){
	FILE* f;
	int res;

	res = Down_Semaforo(semid, 1, SEM_UNDO);
	if(res == -1){
		printf("Error al bajar semaforo de fichero.\n");
	}

	f = fopen("fichero.txt", "a");
	if(f == NULL){
		printf("Error al abrir el fichero.\n");
	}

	fprintf(f, "%s", string);

	fclose(f);

	res = Up_Semaforo(semid, 1, SEM_UNDO);
	if(res == -1){
		printf("Error al subir semaforo de fichero.\n");
	}
}