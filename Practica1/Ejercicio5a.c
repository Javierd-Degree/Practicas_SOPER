#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PROC 6

int main(void) {
	int pid;
	int i;
	int status;

	for (i=0; i <= NUM_PROC; i++){
		if (i % 2 != 0) {
			if ((pid=fork()) <0 ){
				printf("Error al emplear fork\n");
				exit(EXIT_FAILURE);
			}else if (pid ==0){
				printf("HIJO %d, DEL PADRE %d\n", getpid(), getppid());
			}else{
				printf ("PADRE %d \n", getpid());
				/*Esperamos a que su proceso hijo termine*/
				waitpid(pid, &status, WUNTRACED | WCONTINUED);
				break;
			}
		}
	}

	wait(NULL);
	exit(EXIT_SUCCESS);
}	

/*
Cambios

Modificamos la condicion del if cambiando == por != como se pedia
En el caso de que el proceso sea el padre, hacemos que espere a que su hijo acabe
y una vez su hijo ha acabado, sale del bucle para evitar crear mas hijos.

*/