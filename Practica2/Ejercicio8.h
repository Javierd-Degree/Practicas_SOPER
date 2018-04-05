/**
* @brief Ejercicio 8 de la Practica 2.
*
* @file Ejercicio8.h
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/

#ifndef EJERCICIO8_H
#define EJERCICIO8_H

#include <sys/sem.h>

#define ERROR -1
#define OK 0
#define CREADO 1

/**
* @brief Función que inicializa un array de semaforos con id semid y con los valores
* que se pasan a traves del array de shorts.
*
* @param semid Int que es el id del array de semaforos.
* @param array Array de shorts con la informacion para inicializar los semaforos.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
int Inicializar_Semaforo(int semid, unsigned short *array);

/**
* @brief Función que elimina un array de semaforos.
*
* @param semid Int que es el id del array de semáforos.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
int Borrar_Semaforo(int semid);

/**
* @brief Funcion que crea un array de semaforos.
*
* @param key Identificador de IPC.
* @param size Tamano del array de semaforos que se quiere crear.
* @param semid Int que es el id del array de semáforos.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
int Crear_Semaforo(key_t key, int size, int *semid);int Down_Semaforo(int id, int num_sem, int undo);

/**
* @brief Funcion que baja un semaforo.
*
* @param id Int con el id del array de semaforos.
* @param num_sem Int con el indice del semaforo que se quiere bajar.
* @param undo Int que es la bandera que hay que usar en la estructura sem_oper.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
int Down_Semaforo(int id, int num_sem, int undo);

/**
* @brief Funcion que baja varios semaforos de un array llamando varias veces a Down_Semaforo.
*
* @param id Int con el id del array de semaforos.
* @param size Int con el tamano del array de semaforos.
* @param undo Int que es la bandera que hay que usar en la estructura sem_oper.
* @param active Array de int con los indices de los semaforos que se quiere bajar.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
int DownMultiple_Semaforo(int id,int size,int undo,int *active);

/**
* @brief Funcion que sube un semaforo.
*
* @param id Int con el id del array de semaforos.
* @param num_sem Int con el indice del semaforo que se quiere subir.
* @param undo Int que es la bandera que hay que usar en la estructura sem_oper.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
int Up_Semaforo(int id, int num_sem, int undo);

/**
* @brief Funcion que sube varios semaforos de un array llamando varias veces a Up_Semaforo.
*
* @param id Int con el id del array de semaforos.
* @param size Int con el tamano del array de semaforos.
* @param undo Int que es la bandera que hay que usar en la estructura sem_oper.
* @param active Array de int con los indices de los semaforos que se quiere subir.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
int UpMultiple_Semaforo(int id,int size, int undo, int *active);

#endif /* EJERCICIO8_H */