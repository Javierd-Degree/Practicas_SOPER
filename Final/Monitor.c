/**
* @brief Monitor del Proyecto Final.
*
* @file Monitor.c
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/

#include "Monitor.h"

/**
* @brief Funcion que ejecutara el proceso monitor antes del comienzo de la carrera, imprime una cuenta atras con cuanto queda para 
* el comienzo de la carrera e imprime cada segundo como van las cotizaciones de los caballos.
* @param numCaballos int que indica el numero de caballos que participan en la carrera.
* @param memid int que indica el id de la memoria compartida que contiene los datos de las cotizaciones.
* @param semid int que indica el id del array de semaforos que controla la memoria compartida.
*/
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
			syslog(LOG_NOTICE, "Soy el monitor y he impreso la cuenta atras y las cotizaciones de los caballos.\n");
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

/**
* @brief Funcion que ejecutara el proceso monitor durante la carrera,realiza mediante semaforos un rendezvous con el proceso principal 
* que ejecuta la carrera para sincronizarse y va imprimiendo por pantalla la posicion de los caballos en cada momento y las tiradas que
* van realizando.
* @param numCaballos int que indica el numero de caballos que participan en la carrera.
* @param memid int que indica el id de la memoria compartida que contiene los datos de las cotizaciones.
* @param semid int que indica el id del array de semaforos.
* @param longCarrera int que indica la longitud de la carrera.
*/
void monitorDuranteCarrera(int semid, int memid, int numCaballos, int longCarrera){
	int i;
	int res;
	int *memCaballos;
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
		syslog(LOG_NOTICE, "Soy el monitor y he impreso la tirada de los caballos.\n");
		res = Up_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo");
		}

		/*Rendezvous 2.*/
		res = Up_Semaforo(semid, 3, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo del rendezvous 2.");
		}
		/*Hacemos un break para que el monitor salga del bucle pero dejamos el up del rendezvous 
		para que el proceso principal pueda salir del rendezvous en la ultima iteracion de la carrera*/
		if(ganador) break;
		res = Down_Semaforo(semid, 4, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semáforo del rendezvous 2.");
		}
	}

	shmdt(memCaballos);
	return;
}

/**
* @brief Funcion que ejecutara el proceso monitor al acabar la carrera, imprime la posicion en que han quedado los caballos asi
* como una lista con los 10 apostadore con mas beneficios obtenidos, ademas imprime en un fichero los dator de todas las apuestas
* en orden de ralizacion.
* @param numCaballos int que indica el numero de caballos que participan en la carrera.
* @param semid int que indica el id del array de semaforos que controla la memoria compartida.
* @param memCaballosId int que indica el id de la memoria compartida con la posicion de los caballos.
* @param memApostadoresId int que indica el id de la memoria compartida con los datos de los apostadores y las apuestas.
* @param numApostadores int que indica el numero de personas que apuestan en la carrera.
* @param dineroApostador int que indica el dinero del que disponen los apostadores.
* @param longCarrera int que indica la longitud de la carrera.
*/
void monitorDespuesCarrera(int memCaballosId, int memApostadoresId, int numCaballos, int numApostadores, int dineroApostador, int longCarrera, int semid){
	int i;
	int j = 0;
	int *memCaballos;
	int *ganadores;
	memCompartida* mem;
	double* beneficios;
	double* beneficiosAux;
	double temp;
	char infoApostador[100];
	char resultado [100];

	ganadores = (int*)malloc(sizeof(int)*numCaballos);
	if(ganadores == NULL){
		printf("Error al reservar memoria para ganadores.\n");
	}

	for(i=0; i<numCaballos; i++){
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
		printf("\t -Caballo ganador %d, en posicion %d\n", ganadores[i], memCaballos[ganadores[i]]);
	}

	syslog(LOG_NOTICE, "Soy el monitor y he impreso el resultado de la carrera.\n");



	shmdt(memCaballos);

	/*Imprimimos la informacion de los apostadores.*/

	beneficios = (double*)malloc(sizeof(double)*numApostadores);
	if(beneficios == NULL){
		printf("Error al reservar memoria para beneficios.\n");
	}

	beneficiosAux = (double*)malloc(sizeof(double)*numApostadores);
	if(beneficios == NULL){
		printf("Error al reservar memoria para beneficiosAux.\n");
	}

	mem = (memCompartida*)shmat(memApostadoresId, (char)0, 0);
	if(mem == NULL){
		printf("Error en el shmat de losapostadores.\n");
		exit(EXIT_FAILURE);
	}

	for(i = 1; i<=numApostadores; i++){
		for(j=0; ganadores[j] != -1; j++){
			if(mem->apostadorCaballo[i] == ganadores[j]){
				beneficios[i] = mem->pagar[i] - mem->cantidadApostada[i];
				beneficiosAux[i] = beneficios[i];
				break;
			}else{
				beneficios[i] = 0 - mem->cantidadApostada[i];
				beneficiosAux[i] = beneficios[i];
			}
		}
		sprintf(infoApostador, "Apostador-%d ha apostado %lf y ha obtenido %lf beneficios, por lo que le queda %lf.\n", i, mem->cantidadApostada[i], beneficios[i], dineroApostador + beneficios[i]);
		monitorImprimeReport(infoApostador, semid);
	}

	syslog(LOG_NOTICE, "Soy el monitor y he impreso en el report los resultados de las apuestas.\n");

	for(i=2; i<numApostadores; i++){
		for(j=numApostadores - i; j > 0; j--){
			if(beneficios[j-1] < beneficios[j]){
				temp = beneficios[j];
				beneficios[j] = beneficios[j-1];
				beneficios[j-1] = temp;
			}
		}
	}

	printf("Los mejores apostadores han sido: \n");

	for(i = 0; i<10 && beneficios[i] > 0 ; i++){
		for(j=0; j<numApostadores; j++){
			if(beneficios[i] == beneficiosAux[j]){
				printf("\tApostador-%d ha obtenido un beneficio de %lf\n", j, beneficios[i]);
				break;
			}
		}
	}

	syslog(LOG_NOTICE, "Soy el monitor y he impreso los mejores apoestadores.\n");




	shmdt(mem);
	free(beneficios);
	free(beneficiosAux);
	free(ganadores);

	printf("Se ha generado un informe de la carrera que se puede ver en el archivo report.txt\n");

	syslog(LOG_NOTICE, "Soy el monitor y he acabado el report.\n");
	

	return;
}


/**
* @brief Funcion que imprime una string dada en el fichero del report.
* @param string, cadena de caracteres que se imprimira en el fichero.
* @param semid int que indica el id del array de semaforos que controla el acceso al fichero.
*/
void monitorImprimeReport(char* string, int semid){
	FILE* f;
	int res;

	res = Down_Semaforo(semid, 1, SEM_UNDO);
	if(res == -1){
		printf("Error al bajar semaforo de fichero.\n");
	}

	f = fopen("report.txt", "a");
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