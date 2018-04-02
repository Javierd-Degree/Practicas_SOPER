#ifndef EJERCICIO8_H
#define EJERCICIO8_H

#include <sys/sem.h>

#define ERROR -1
#define OK 0
#define CREADO 1

int Inicializar_Semaforo(int semid, unsigned short *array);
int Borrar_Semaforo(int semid);
int Crear_Semaforo(key_t key, int size, int *semid);int Down_Semaforo(int id, int num_sem, int undo);
int DownMultiple_Semaforo(int id,int size,int undo,int *active);
int Up_Semaforo(int id, int num_sem, int undo);
int UpMultiple_Semaforo(int id,int size, int undo, int *active);

#endif /* EJERCICIO8_H */