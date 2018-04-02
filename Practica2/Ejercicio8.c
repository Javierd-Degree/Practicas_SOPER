#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <stdlib.h>
#include "Ejercicio8.h"

#define SEMKEY 75798
#define N_SEMAFOROS 2

/*En este caso, guardamos en una varible global dentro de la libreria el numero
de semaforos que tenemos, de forma que el usuario no es consciente de esto.*/

union semun {
	int val;
	struct semid_ds *semstat;
	unsigned short *array;
} arg;

int n_sems;

int Inicializar_Semaforo(int semid, unsigned short * array){
	int i;
	if(array == NULL){
		return ERROR;
	}

	/*Borramos para asegurarnos de no perder memoria*/
	if(arg.array != NULL){
		free(arg.array);
	}

	arg.array = (unsigned short *)malloc(sizeof(unsigned short) * n_sems);
	if(arg.array == NULL){
		return ERROR;
	}

	for(i = 0; i < n_sems; i++){
		arg.array[i] = array[i];
	}

	/*Se le pasa el semid del array de semaforos, el número de semaforos y SETALL 
	para que los inicialice todos, inicializa el array de semaforos con los valores de arg.array*/
	semctl (semid, n_sems, SETALL, arg);
	return OK;
}


int Borrar_Semaforo(int semid){
	semctl (semid, 0, IPC_RMID);
	free(arg.array);
	return OK;
}


int Crear_Semaforo(key_t key, int size, int* semid){
	int ret;
	*semid = semget(key, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
	if((*semid == -1) && (errno == EEXIST)){
		/*Ya esta creado, pero necesitamos el id de verdad, no -1*/
		*semid = semget(key, size, SHM_R | SHM_W);
		printf("Ya esta creado\n");
		return CREADO;
	}
	if(*semid==-1){
		perror("semget");
		return ERROR;
	}

	/*Inicializamos el array con una posicion mas, que es la maxima que admite un unsigned short,
	para saber hasta donde escribir al inicializar etc.*/
	n_sems = size;
	unsigned short* array = (unsigned short*)calloc(size, sizeof(unsigned short));
	
	ret = Inicializar_Semaforo(*semid, array);
	free(array);
	return ret;	
}

int Down_Semaforo(int id, int num_sem, int undo){
	struct sembuf sem_oper;
	int ret;
	sem_oper.sem_num = num_sem; /* Actuamos sobre el semáforo num_sem de la lista */
	sem_oper.sem_op = -1; /* Decrementar en 1 el valor del semáforo */
	sem_oper.sem_flg = SEM_UNDO; /* Para evitar interbloqueos si un
	proceso acaba inesperadamente */
	ret = semop(id, &sem_oper, 1);
	if(ret == -1){
		return ERROR;
	}
	return OK;
}

int DownMultiple_Semaforo(int id, int size, int undo, int* active){
	int ret, i;
	for(i = 0; i < sizeof(active)/sizeof(int); i++){
		ret = Down_Semaforo(id, active[i], undo);
		if(ret == ERROR){
			return ERROR;
		}
	}
	return OK;
}

int Up_Semaforo(int id, int num_sem, int undo){
	struct sembuf sem_oper;
	int ret;
	sem_oper.sem_num = num_sem; /* Actuamos sobre el semáforo 0 de la lista */
	sem_oper.sem_op =1; /* Decrementar en 1 el valor del semáforo */
	sem_oper.sem_flg = SEM_UNDO; /* Para evitar interbloqueos si un
	proceso acaba inesperadamente */
	ret = semop (id, &sem_oper, 1);
	if(ret == -1){
		return ERROR;
	}
	return OK;
}

int UpMultiple_Semaforo(int id, int size, int undo, int* active){
	int ret, i;
	for(i = 0; i < sizeof(active)/sizeof(int); i++){
		ret = Up_Semaforo(id, active[i], undo);
		if(ret == ERROR){
			return ERROR;
		}
	}
	return OK;
}

int test(){
	/*
	* Declaración de variables
	*/
	int semid, ret; /* ID de la lista de semáforos */
	unsigned short array[N_SEMAFOROS] = {1, 1};

	/*
	* Creamos una lista o conjunto con dos semáforos
	*/
	ret = Crear_Semaforo(SEMKEY, N_SEMAFOROS, &semid);
	if(ret == ERROR){
		exit(EXIT_FAILURE);
	}

	printf("Semid %d\n", semid);

	/*
	* Inicializamos los semáforos
	*/
	ret = Inicializar_Semaforo(semid, array);
	if(ret == ERROR){
		exit(EXIT_FAILURE);
	}

	/*
	* Operamos sobre los semáforos
	*/
	ret = Down_Semaforo(semid, 0, SEM_UNDO);
	if(ret == ERROR){
		exit(EXIT_FAILURE);
	}
	ret = Up_Semaforo(semid, 1, SEM_UNDO);
	if(ret == ERROR){
		exit(EXIT_FAILURE);
	}

	/*
	* Veamos los valores de los semáforos
	*/
	semctl (semid, N_SEMAFOROS, GETALL, arg);
	printf ("Los valores de los semáforos son %d y %d\n", arg.array [0], arg.array
	[1]);
	/* Eliminar la lista de semáforos */
	ret = Borrar_Semaforo(semid);
	if(ret == ERROR){
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
