#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*#include  <sys/types.h>*/


#define NUM_PROC 6

int main (void) {
	int pid;
	int i;
	int status;

	for (i=0; i <= NUM_PROC; i++){
		if (i % 2 == 0) {
			if ((pid=fork()) <0 ){
				printf("Error al emplear fork\n");
				exit(EXIT_FAILURE);
			}else if (pid ==0){
				printf("HIJO %d, DEL PADRE %d\n", pid, getppid());
			}else{
				printf ("PADRE %d \n", getppid());
			}
		}
	}

	pid = wait(&status);
	/*printf("El hijo %d de padre %d ha acabado con status %d\n" pid, getpid(), status);*/
	exit(EXIT_SUCCESS);
}	

/*
DIBUJO DE LA HOJA

Los procesos se quedan huerfanos cuando el padre sale del bucle, con lo que el padre tiene id = 1
En el caso b sin embargo, los procesos no se quedan huerfanos, porque el padre espera, excepto los 
dos procesos que por alguna razon se quedan huerfanos.


Al ejercutar pstree para los procesos padre, al estar ya todos los hijos y el mismo padre muertos, 
no obtenemos ninguna salida.
*/
