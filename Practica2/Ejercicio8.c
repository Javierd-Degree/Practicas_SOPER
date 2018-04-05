/**
* @brief Ejercicio 8 de la Practica 2.
*
* @file Ejercicio8.c
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/

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


/**
* @brief Función que inicializa un array de semaforos con id semid y con los valores
* que se pasan a traves del array de shorts.
*
* @param semid Int que es el id del array de semaforos.
* @param array Array de shorts con la informacion para inicializar los semaforos.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
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

/**
* @brief Función que elimina un array de semaforos.
*
* @param semid Int que es el id del array de semáforos.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
int Borrar_Semaforo(int semid){
	if(semctl (semid, 0, IPC_RMID) == -1){
		return ERROR;
	}
	return OK;
}

/**
* @brief Funcion que crea un array de semaforos.
*
* @param key Identificador de IPC.
* @param size Tamano del array de semaforos que se quiere crear.
* @param semid Int que es el id del array de semáforos.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
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

/**
* @brief Funcion que baja un semaforo.
*
* @param id Int con el id del array de semaforos.
* @param num_sem Int con el indice del semaforo que se quiere bajar.
* @param undo Int que es la bandera que hay que usar en la estructura sem_oper.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
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

/**
* @brief Funcion que baja varios semaforos de un array llamando varias veces a Down_Semaforo.
*
* @param id Int con el id del array de semaforos.
* @param size Int con el tamano del array de semaforos.
* @param undo Int que es la bandera que hay que usar en la estructura sem_oper.
* @param active Array de int con los indices de los semaforos que se quiere bajar.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
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


/**
* @brief Funcion que sube un semaforo.
*
* @param id Int con el id del array de semaforos.
* @param num_sem Int con el indice del semaforo que se quiere subir.
* @param undo Int que es la bandera que hay que usar en la estructura sem_oper.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
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

/**
* @brief Funcion que sube varios semaforos de un array llamando varias veces a Up_Semaforo.
*
* @param id Int con el id del array de semaforos.
* @param size Int con el tamano del array de semaforos.
* @param undo Int que es la bandera que hay que usar en la estructura sem_oper.
* @param active Array de int con los indices de los semaforos que se quiere subir.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
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


/**
* @brief Programa que prueba el funcionamiento de la libreria.
*
* @return int que determina si el programa se ha ejecutado o no con exito.
*/

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
