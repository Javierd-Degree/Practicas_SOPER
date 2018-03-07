/**
* @brief Apartado a del ejercicio 5.
* Modificamos el ejercicio 4 minimamentepara que el proceso padre genere hijos
* para los numeros i impares y los espere.
*
* @file Ejercicio5b.c
* @author Javier.delgadod@estudiante.uam.es Javier.lopezcano@estudiante.uam.es
*/

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PROC 6 /*!< Numero de veces que se recorre el bucle*/


/**
* @brief Crea un arbol de procesos de forma que el padre
* crea varios hijos, y se asegura de esperarlos.
* Los hijos no pueden crear mas procesos.
*
*/
int main() {
	int pid;
	int i;
	int status;

	for (i=0; i <= NUM_PROC; i++){
		if (i % 2 != 0) {
			if ( pid !=0 && (pid=fork()) <0 ){
				printf("Error al emplear fork\n");
				exit(EXIT_FAILURE);
			}else if (pid ==0){
				printf("HIJO %d, DEL PADRE %d\n", getpid(), getppid());
			}else{
				/*Esperamos a su proceso hijo, pero sin bloquear el padre
				de forma que podamos seguir creando el resto de procesos. */
				waitpid(pid, &status, WUNTRACED | WCONTINUED);
			}
		}
	}
	
	exit(EXIT_SUCCESS);
}