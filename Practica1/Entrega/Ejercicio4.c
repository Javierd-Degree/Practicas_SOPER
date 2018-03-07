/**
* @brief Ejercicio 4 con la modificacion necesaria para que cada hijo imprima
* su pid y el pid de su padre
*
* @file Ejercicio4.c
* @author Javier.delgadod@estudiante.uam.es Javier.lopezcano@estudiante.uam.es
*/

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PROC 6 /*!< Numero de veces que se recorre el bucle*/

/**
* @brief Crea un arbol de procesos de forma que cada hijo imprime
* su pid y el pid de su padre.
*
* @return int que determina si el programa se ha ejecutado o no con exito.
*/
int main(){
	int pid;
	int i;
	int status;

	for (i=0; i <= NUM_PROC; i++){
		if (i % 2 == 0) {
			if ((pid=fork()) <0 ){
				printf("Error al emplear fork\n");
				exit(EXIT_FAILURE);
			}else if (pid ==0){
				printf("HIJO %d, DEL PADRE %d\n", getpid(), getppid());
			}else{
				/*printf ("PADRE %d \n", getpid());*/
			}
		}
	}

	pid = wait(&status);
	exit(EXIT_SUCCESS);
}
