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

/*Creamos la estructura de los semaforos*/
union semun {
	int val;
	struct semid_ds *semstat;
	unsigned short *array;
} arg;


int Inicializar_Semaforo(int semid, unsigned short * array){
	if(array == NULL){
		return ERROR;
	}

	arg.array = array;
	/*Se le pasa el semid del array de semaforos, el número de semaforos y SETALL 
	para que los inicialice todos, inicializa el array de semaforos con los valores de arg.array*/
	if(semctl (semid, 0, SETALL, arg) == -1){
		return ERROR;
	}
	return OK;
}


int Borrar_Semaforo(int semid){
	if(semctl (semid, 0, IPC_RMID) == -1){
		return ERROR;
	}
	return OK;
}


int Crear_Semaforo(key_t key, int size, int* semid){
	if(size < 0 || semid == NULL){
		return ERROR;
	}

	*semid = semget(key, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);

	if((*semid == -1) && (errno == EEXIST)){
		/*Ya esta creado, pero necesitamos el id de verdad, no -1*/
		*semid = semget(key, size, SHM_R | SHM_W);
		return CREADO;

	}else if(*semid == -1){
		perror("semget");
		return ERROR;
	}
	
	return OK;
}

int Down_Semaforo(int id, int num_sem, int undo){
	struct sembuf sem_oper;
	int ret;

	if(num_sem < 0){
		return ERROR;
	}

	sem_oper.sem_num = num_sem; /* Actuamos sobre el semáforo num_sem de la lista */
	sem_oper.sem_op = -1; /* Decrementar en 1 el valor del semáforo */
	sem_oper.sem_flg = undo; /* Para evitar interbloqueos si un
	proceso acaba inesperadamente */
	ret = semop(id, &sem_oper, 1);
	if(ret == -1){
		return ERROR;
	}
	return OK;
}

int DownMultiple_Semaforo(int id, int size, int undo, int* active){
	int ret, i;

	if(active == NULL || size < 0){
		return ERROR;
	}

	for(i = 0; i < size; i++){
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
	for(i = 0; i < size; i++){
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
	printf("Inicializamos semaforos\n");
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
