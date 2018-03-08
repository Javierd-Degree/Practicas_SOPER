/**
* @brief Apartado a del ejercicio 5.
* Modificamos el ejercicio 4 minimamentepara que se generen procesos para
* los numeros i impares, de forma que cada padre tiene un unico hijo y le
* tiene que esperar.
*
* @file Ejercicio5a.c
* @author Javier.delgadod@estudiante.uam.es Javier.lopezcano@estudiante.uam.es
*/

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PROC 6 /*!< Numero de veces que se recorre el bucle*/


/**
* @brief Crea un arbol de procesos de forma que cada proceso
* crea un unico hijo, y se asegura de esperarle.
*
*/
int main() {
	int pid;
	int i;

	for (i=0; i <= NUM_PROC; i++){
		if (i % 2 != 0) {
			if ((pid=fork()) <0 ){
				printf("Error al emplear fork\n");
				exit(EXIT_FAILURE);
			}else if (pid ==0){
				printf("HIJO %d, DEL PADRE %d\n", getpid(), getppid());
			}else{
				/*Salimos, de forma que dejamos de crear hijos.
				Al salir, hacemos el wait y esperamos a que el hijo termine*/
				break;
			}
		}
	}

	wait(NULL);
	exit(EXIT_SUCCESS);
}