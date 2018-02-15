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
			if ( pid !=0 && (pid=fork()) <0 ){
				printf("Error al emplear fork\n");
				exit(EXIT_FAILURE);
			}else if (pid ==0){
				printf("HIJO %d, DEL PADRE %d\n", getpid(), getppid());
			}else{
				printf ("PADRE %d \n", getpid());
				/*Esperamos a que su proceso hijo termine*/
				waitpid(pid, &status, WUNTRACED | WCONTINUED);
			}
		}
	}

	wait(NULL);
	exit(EXIT_SUCCESS);
}	

/*
Cambios

Modificamos la condicion del if cambiando == por != como se pedia
Al hacer fork, comprobamos antes que el pid guardado no sea 0, de esta forma
solo el padre crea nuevos procesos.

En el caso de que el proceso sea el padre, hacemos que espere a que su hijo acabe
y una vez su hijo ha acabado, continuamos con el bucle.


TODO Hacer que no haya que esperar a los hijos dentro del propio bucle, sino fuera,
usando wait.

*/