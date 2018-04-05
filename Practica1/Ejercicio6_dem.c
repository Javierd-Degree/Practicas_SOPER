/*
Queremos demostrar que al hacer fork el hijo recibe una copia exacta, 
pero independiente, de la informaciion que el padre tiene guardada.
*/

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_PROC 6

typedef struct _estructura{
	char cad[80];
	int num;
}estructura;

int main(void) {
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
		waitpid(pid, &status, WUNTRACED | WCONTINUED);
		printf("\nEl proceso hijo ha acabado\n");
		printf("La informacion actual es:\n\t%s, %d\n", est->cad, est->num);
	}

	free(est);
	exit(EXIT_SUCCESS);
}