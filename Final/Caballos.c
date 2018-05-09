/**
* @brief Caballos de la practica final.
*
* @file Caballos.c
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/


#include "Caballos.h"
#include "Utils.h"
#include <errno.h>

/**
* @brief Funcion que nos devuelve el numero de
* casillas que tiene que avanzar un caballo segun
* su relativa posicion en la lista.
*
* @param modo Posicion relativa en la lista
* @return numero de casillas que avanza
*/
int caballoAvanza(int modo){
	if(modo == MEDIO){
		return aleatNum(1, 6);
	}else if(modo == PRIMERO){
		return aleatNum(1, 7);
	}else if(modo == ULTIMO){
		return aleatNum(2, 12);
	}

	return -1;
}

/**
* @brief Funcion llamada por el proceso principal
* para hacer la carrera de los caballos.
*
* @param numCaballos numero de caballos de la carrera
* @param longCarrera longitud de la carrera
* @param semid id del array de semaforos que controla los caballos
* @param memid id de la memoria compartida de los caballos
*/
void carrera(int numCaballos, int longCarrera, int semid, int memid){
	int i;
	int res;
	int pids[MAX_CABALLOS];
	int *memCaballos;
	int pipesCaballos[20][2];
	int ganador = 0;

	char temp[124];

	/*Guardamos en primer lugar todas las posiciones, en segundo 
	lugar todas las tiradas.*/
	memCaballos = (int*)shmat(memid, (char*)0, 0);
	if(memCaballos == NULL) {
		printf ("Error reserve shared memory \n");
		exit(EXIT_FAILURE);
	}

	/*Inicializamos todos los pipes.*/
	for(i = 0; i < numCaballos; i++){
		if(pipe(pipesCaballos[i]) == -1 || pipe(pipesCaballos[numCaballos + i]) == -1){
			printf("Error al inicializar los pipes.\n");
			exit(EXIT_FAILURE);
		}
	}

	/*Creamos todos los caballos.*/
	for(i = 0; i < numCaballos; i++){
		pids[i] = fork();
		if(pids[i] == -1){
			printf("Error al hacer fork.\n");
			exit(EXIT_FAILURE);
		}else if(pids[i] == 0){
			/* CAMBIAR FUNCION.*/
			caballo(i+1, pipesCaballos[i], pipesCaballos[numCaballos + i], longCarrera);
			exit(EXIT_SUCCESS);
		}else{
			printf("ID Caballo %d: %d\n", i+1, pids[i]);
			close(pipesCaballos[i][0]);
			close(pipesCaballos[numCaballos + i][1]);
		}
	}

	/*Empieza la carrera*/
	while(!ganador){
		/*Rendezvous*/
		res = Down_Semaforo(semid, 1, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semáforo del rendezvous.");
		}

		res = Up_Semaforo(semid, 2, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo del rendezvous.");
		}

		/*Hacemos las tiradas de todos los caballos.*/
		res = Down_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semáforo");
		}


		for(i = 0; i < numCaballos; i++){
			/*Le mandamos a cada caballo la posicion en
			la que esta, para que haga su tirada.*/
			sprintf(temp, "%d", posicionCaballo(i, memCaballos, numCaballos));
			write(pipesCaballos[i][1], temp, strlen(temp)+1);
		}

		res = Up_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo");
		}

		/*Esperamos a todos los caballos y actualizamos
		sus posiciones*/
		res = Down_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semáforo");
		}

		for(i = 0; i < numCaballos; i++){
			read(pipesCaballos[numCaballos + i][0], temp, 124);
			/*Guardamos la tirada y actualizamos la posicion.*/
			memCaballos[numCaballos + i] = atoi(temp);
			memCaballos[i] += atoi(temp);

			if(memCaballos[i] >= longCarrera){
				ganador = 1;
			}
		}
		res = Up_Semaforo(semid, 0, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo");
		}

		/*Rendezvous 2*/
		res = Down_Semaforo(semid, 3, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semáforo del rendezvous 2.");
		}
		res = Up_Semaforo(semid, 4, SEM_UNDO);
		if(res == -1){
			printf("Error al subir el semáforo del rendezvous 2.");
		}
	}

	/*Hacemos un ultimo up para que el monitor imprima la ultima tirada.*/
	res = Up_Semaforo(semid, 2, SEM_UNDO);
	if(res == -1){
		printf("Error al subir el semáforo del rendezvous.");
	}

	/*Finalizamos todos los caballos*/
	for(i = 0; i < numCaballos; i++){
		kill(pids[i], SIGKILL);
		wait(NULL);
	}

	shmdt(memCaballos);
}

/**
* @brief Funcion que permite inicializar los recursos de los caballos.
*
* @param recursosCaballo puntero a la estructura donde se guardaran 
* los recursos
* @param numCaballos numero de caballos 
* @return 0 si todo ha ido bien, -1 si hay algun error
*/
int inicializaRecursosCaballo(recursosCaballo *r, int numCaballos){
	int semid;
	int memid;
	int i;
	int res;
	unsigned short mutexValor[] = {1, 0, 0, 0, 0};
	int *temp;

	if(r == NULL){
		return -1;
	}
	/*Creamos e inicializamos el semáforo que controlará la entrada a 
	la memoria compartida de los caballos.
	*/

	
	int semkey = ftok(FILE_CABALLO_KEY, CABALLO_KEY);
	if(semkey ==(key_t) -1){
		printf("Error al obtener la clave de los semaforos. %s", strerror(errno));
		return -1;
	}

	res = Crear_Semaforo(semkey, 5,  &semid);
	if(res == -1){
		printf("Error al crear el array de semáforos %s\n", strerror(errno));
		return -1;
	}
	res = Inicializar_Semaforo(semid, mutexValor);
	if(res == -1){
		printf("Error al inicializar el array de semáforos %s\n", strerror(errno));
		return -1;
	}

	/*Inicializamos la memoria compartida de las posiciones y de las tiradas.*/
	int memkey = ftok(FILE_CABALLO_MEM_KEY, CABALLO_MEM_KEY);
	if(memkey ==(key_t) -1){
		printf("Error al obtener la clave de la memoria compartida. %s", strerror(errno));
		return -1;
	}
	memid = shmget(memkey, 2*numCaballos*sizeof(int), IPC_CREAT | SHM_W | SHM_R);
	if(memid == -1){
		printf("Error al reservar la memoria compartida para los caballos. %s\n", strerror(errno));
	}
	/*Inicializamos la memoria a cero*/
	temp = (int*)shmat(memid, (char*)0, 0);
	if(temp == NULL) {
		printf ("Error con la memoria compartida\n");
		return -1;
	}

	for(i = 0; i < numCaballos; i++){
		temp[i] = 0;
	}

	/*Liberamos el direccionamiento virtual de la memoria compartida*/
	shmdt(temp);

	r->memid = memid;
	r->semid = semid;

	return 0;
}

/**
* @brief Funcion que permite inicializar los recursos de los caballos.
*
* @param recursosCaballo puntero a la estructura con los recursos a liberar 
* @return 0 si todo ha ido bien, -1 si hay algun error
*/
int liberarRecursosCaballo(recursosCaballo *r){
	if(r == NULL){
		return -1;
	}

	shmctl (r->memid, IPC_RMID, (struct shmid_ds *)NULL);

	/*Liberamos el array de semáforos*/
	if(Borrar_Semaforo(r->semid) == -1){
		printf("Error al liberar el semáforo");
		return -1;
	}
	return 0;
}

/**
* @brief Dada la lista de caballos, nos permite saber si un caballo
* esta en el medio, al final o al principio.
*
* @param num numero del caballo
* @param lista Lista de las posiciones de los caballos
* @param numCaballos numero de caballos 
* @return MEDIO, PRIMERO o ULTIMO
*/
int posicionCaballo(int num, int *lista, int numCaballos){
	int i;
	int esMax = 1;
	int esMin =  1;

	/*En la primera tirada tiramos un dado normal.*/
	if(lista[num] == 0){
		return MEDIO;
	}

	for(i = 0; i < numCaballos; i++){
		if(i == num) continue;
		if(lista[num] <= lista[i]){
			esMax = 0;
		}else if(lista[num] > lista[i]){
			esMin = 0;
		}
	}

	if(esMax == 1){
		return PRIMERO;
	}else if(esMin == 1){
		return ULTIMO;
	}

	return MEDIO;
}

/**
* @brief Funcion que ejecuta un caballo
*
* @param numero numero del caballo
* @param pipe Pipe que usa el caballo para leer
* @param pipe Pipe2 que usa el caballo para escribir
* @param lonCarrera longitud de la carrera
*/
void caballo(int numero, int pipe[2], int pipe2[2], int lonCarrera){
	int posicion;
	int avanza;
	char temp[124];


	/*Inicializamos la semilla para generar los numeros aleatorios*/
	srand(time(0)*numero);

	/*El caballo solo puede leer del pipe y escribir en pipe2*/
	close(pipe[1]);
	close(pipe2[0]);

	while(1){
		/*El padre le avisa cuando ha escrito en el pipe*/
		//printf("Soy el caballo %d, quiero leer.\n", numero);
		read(pipe[0], temp, 124);
		posicion = atoi(temp);

		avanza = caballoAvanza(posicion);
		sprintf(temp, "%d", avanza);

 		//printf("Soy el caballo %d llevo %d\n", numero, recorrido);
 		//printf("Soy el caballo %d, voy en posicion %d, y avanzo %d\n", numero, posicion, avanza);

 		write(pipe2[1], temp, strlen(temp)+1);
	}

	return;
}
