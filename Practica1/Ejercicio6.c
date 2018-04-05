#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
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

	if((pid = fork()) < 0){
		printf("Error al emplear fork\n");
		exit(EXIT_FAILURE);
	}else if(pid == 0){
		printf("Introduce el texto deseado: ");
		scanf("%s", est->cad);
	}else{
		/*El padre espera a que el hijo acabe para intentar acceder al valor*/
		waitpid(pid, &status, WUNTRACED | WCONTINUED);
		printf("El proceso hijo ha acabado\n");
		printf("El texto que has introducido es: %s\n", est->cad);
	}

	free(est);
	exit(EXIT_SUCCESS);
}

/*
Intentamos imprimir en el proceso padre, una vez que el proceso hijo 
ha capturado el texto y ha acabado, el texto que el proceso hijo ha 
guardado en el struct, sin embargo no obtenemos nada.
Usando valgrind, probamos a liberar la memoria en el if del proceso padre
unicamente,(pid > 0) en el if del proceso hijo unicamente (pid==0), o en el 
main, antes del exit(EXIT_SUCCESS), y podemos ver asi que la unica forma de
no perder memoria es libernadola fuera de los condicionales, antes del exit,
o bien dentro de los dos condicionales.

Esto se debe a que al llamar a fork, se crea una copia exacta e independiente
del proceso que estamos ejecutando, con las variables incluidas. Por tanto, 
el hijo y el padre estan accediendo a posiciones de memoria distintas.
Esto es lo que nos permite, como se ve en el fichero Ejercicio6_dem, acceder a 
una copia de la informacion que el padre ha guardado en el struct antes de crear el hijo.
*/