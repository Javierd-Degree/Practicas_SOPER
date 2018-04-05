#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

double potencia(int a, int b){
	return pow(a, b);
}

/*Devuelve a!/b*/
double factorial(int a, int b){
	int i;
	double result;

	for(i = 1, result = 1; i <= a; i++){
		result *= i;
	}

	return result/b;
}

/* Devuelve el numero combinatorio a sobre b
TODO Buscar otra implementacion no recursiva
*/
double numCombinatorio(int a, int b){

	if( a < 0 || b < 0) return -1;
	else if(b == 0 || a == b) return 1;
	else if(b > a) return 0;
	else{
		return numCombinatorio(a-1, b-1) +  numCombinatorio(a-1, b);
	}
}

double sumaAbsoluta(int a, int b){
	return fabs(a) + fabs(b);
}
	
int	main(void){	
	int	fd[2], fe[2], ff[2], fg[2], fh[2], fi[2], fj[2], fk[2];
	int a, b;
	int i;	
	pid_t pid[4];	
	char string[20];	
	char readbuffer[256];
	double result;

	/*Creamos las pipes*/
	if(pipe(fd) == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	if(pipe(fe) == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	if(pipe(ff) == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	if(pipe(fg) == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	if(pipe(fh) == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	if(pipe(fi) == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	if(pipe(fj) == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	if(pipe(fk) == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}


	/*Pedimos por pantalla los valores de O1 y O2*/
	printf("Introduzca 2 enteros separados por una coma: ");
	scanf("%s", string);

	/*Hacemos los fork para lanzar los procesos hijo*/
	for(i = 0; i < 4; i++){
		if((pid[i] = fork()) == -1){
			perror("Error al hacer fork");
			exit(EXIT_FAILURE);
		}else if(pid[i] == 0){
			break;
		}
	}
	
	
	/*El primer hijo realiza la potencia*/	
	if(pid[0] == 0){		
		close(fd[1]);
		/*Leer los datos del pipe e imprimirlos*/	
		read(fd[0], readbuffer, sizeof(readbuffer));
		/*Separar la cadena obtenida para separar los 2 datos y pasarlos a ints*/
		sscanf(readbuffer, "%d,%d", &a, &b);
		/*Calcular la operación*/
		result = potencia(a, b);
		/*Crear la cadena con el resultado*/

		sprintf(readbuffer, "Datos enviados a traves de la tuberia por el proceso PID=%d.\n\tOperando 1: %d. Operando 2: %d. Potencia: %lf\n", getpid(), a, b, result);
		close(fh[0]);
		/*Escribir la nueva cadena en el pipe*/
		write(fh[1], readbuffer, strlen(readbuffer)+1);
		exit(0);

	/*El segundo hijo realiza el factorial*/	
	}else if(pid[1] == 0){	
		close(fe[1]);	
		/*Leer los datos del pipe e imprimirlos*/	
		read(fe[0], readbuffer, sizeof(readbuffer));
		/*Separar la cadena obtenida para separar los 2 datos y pasarlos a ints*/
		sscanf(readbuffer, "%d,%d", &a, &b);
		/*Calcular la operación*/
		result=factorial(a, b);
		/*Crear la cadena con el resultado*/

		sprintf(readbuffer, "Datos enviados a traves de la tuberia por el proceso PID=%d.\n\tOperando 1: %d. Operando 2: %d. Factorial: %lf\n", getpid(), a, b, result);
		close(fi[0]);
		/*Escribir la nueva cadena en el pipe*/
		write(fi[1], readbuffer, strlen(readbuffer)+1);
		exit(0);

	/*El tercer hijo realiza el número combinatorio*/ 
	}else if(pid[2] == 0){	
		close(ff[1]);	
		/*Leer los datos del pipe e imprimirlos*/	
		read(ff[0], readbuffer, sizeof(readbuffer));	
		/*Separar la cadena obtenida para separar los 2 datos y pasarlos a ints*/
		sscanf(readbuffer, "%d,%d", &a, &b);
		/*Calcular la operación*/
		result=numCombinatorio(a, b);
		/*Crear la cadena con el resultado*/
		sprintf(readbuffer, "Datos enviados a traves de la tuberia por el proceso PID=%d.\n\tOperando 1: %d. Operando 2: %d. Numero combinatorio: %lf\n", getpid(), a, b, result);
		close(fj[0]);
		/*Escribir la nueva cadena en el pipe*/
		write(fj[1], readbuffer , strlen(readbuffer)+1);
		exit(0);

	/*El último hijo realiza la suma de valores absolutos*/
	}else if(pid[3] == 0){
		close(fg[1]);	
		/*Leer los datos del pipe e imprimirlos*/	
		read(fg[0], readbuffer, sizeof(readbuffer));	
		/*Separar la cadena obtenida para separar los 2 datos y pasarlos a ints*/
		sscanf(readbuffer, "%d,%d", &a, &b);
		/*Calcular la operación*/	
		result=sumaAbsoluta(a, b);
		/*Crear la cadena con el resultado*/
		sprintf(readbuffer, "Datos enviados a traves de la tuberia por el proceso PID=%d.\n\tOperando 1: %d. Operando 2: %d. Suma absoluta: %lf\n", getpid(), a, b, result);
		close(fk[0]);
		/*Escribir la nueva cadena en el pipe*/
		write(fk[1], readbuffer , strlen(readbuffer)+1);
		exit(0);
	/*El proceso padre escribe los datos y lee el resultado de las pipes*/	
	}else{	
		/* Cierre de los descriptores de salida en el padre*/	
		close(fd[0]);
		close(fe[0]);
		close(ff[0]);
		close(fg[0]);

		/*Cierre de los descriptores de entrada del padre*/
		close(fh[1]);
		close(fi[1]);
		close(fj[1]);
		close(fk[1]);	

		/*Escribir en las tuberías la sitring con O1 y O2*/
		write(fd[1], string, strlen(string)+1);
		write(fe[1], string, strlen(string)+1);	
		write(ff[1], string, strlen(string)+1);
		write(fg[1], string, strlen(string)+1);

		/*Leer de las tuberías las stings con los resultados e imprimirlas*/

		/*Potencia*/
		read(fh[0], readbuffer, sizeof(readbuffer));	
		printf("\n%s\n", readbuffer);

		/*Factorial*/
		read(fi[0], readbuffer, sizeof(readbuffer));
		printf("%s\n", readbuffer);

		/*Número combinatorio*/
		read(fj[0], readbuffer, sizeof(readbuffer));
		printf("%s\n", readbuffer);

		/*Suma de valores absolutos*/
		read(fk[0], readbuffer, sizeof(readbuffer));	
		printf("%s\n", readbuffer);
	}

	return 0;	
}