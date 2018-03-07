/**
* @brief Ejercicio 6.
* Ejercicio que nos permite ver el funcionamiento de la memoria y 
* las variables entre un proceso padre y su hijo.
*
* @file Ejercicio6.c
* @author Javier.delgadod@estudiante.uam.es Javier.lopezcano@estudiante.uam.es
*/


#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/**
* @brief Estructura para duplicar en un proceso y su hijo.
*
* Compuesta con un string de 80 caracteres y un entero.
*/
typedef struct _estructura{
	char cad[80];
	int num;
}estructura;


/**
* @brief Creamos dos procesos e intentamos compartir una variable.
*
* main() Reservamos memoria para una estructura y hacemos fork.
* Modificamos la estructura en el hijo, y cuando acaba, intentamos
* acceder a ella desde el padre.
*/

int main() {
	int pid;
	int status;
	estructura *est;

	est = (estructura*)malloc(sizeof(estructura));
	if(est == NULL){
		exit(EXIT_FAILURE);
	}

	if((pid = fork()) < 0){
		printf("Error al emplear fork\n");
		exit(EXIT_FAILURE);
	}else if(pid == 0){
		printf("Introduce el texto deseado: ");
		scanf("%s", est->cad);
	}else{
		/*El padre espera a que el hijo acabe para intentar acceder al valor*/
		wait(NULL);
		printf("El proceso hijo ha acabado\n");
		printf("El texto que has introducido es: %s\n", est->cad);
	}

	free(est);
	exit(EXIT_SUCCESS);
}