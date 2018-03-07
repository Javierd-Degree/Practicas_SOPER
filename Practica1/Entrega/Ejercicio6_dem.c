/**
* @brief Demostracion de la respuesta aportada en el ejercicio 6.
* Queremos demostrar que al hacer fork el hijo recibe una copia exacta, 
* pero independiente, de la informaciion que el padre tiene guardada.
*
* @file Ejercicio6_dem.c
* @author Javier.delgadod@estudiante.uam.es Javier.lopezcano@estudiante.uam.es
*/

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
* main() Reservamos memoria para una estructura y la inicializamos.
* Hacemos fork, cambiamos la estructura en el proceso hijo y lo acabamos.
* Una vez acabado el proceso hijo intenamos aceder a la informacion desde
* el padre, y podemos ver que el padre no puede acceder a lo modificado
* por el hijo.
*/

int main() {
	int pid;
	int status;
	estructura *est;

	est = (estructura*)malloc(sizeof(estructura));
	if(est == NULL){
		exit(EXIT_FAILURE);
	}

	strcpy(est->cad, "Texto de prueba");
	est->num = 9;

	if((pid = fork()) < 0){
		printf("Error al emplear fork\n");
		exit(EXIT_FAILURE);
	}else if(pid == 0){
		printf("La informacion guardada en el padre es:\n\t%s, %d\n", est->cad, est->num);
		strcpy(est->cad, "Texto modificado");
		est->num = 99;
	}else{
		/*El padre espera a que el hijo acabe para intentar acceder al valor*/
		wait(NULL);
		printf("\nEl proceso hijo ha acabado\n");
		printf("La informacion actual es:\n\t%s, %d\n", est->cad, est->num);
	}

	free(est);
	exit(EXIT_SUCCESS);
}