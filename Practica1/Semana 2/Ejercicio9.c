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
	int pipe_status1, pipe_status2, pipe_status3, pipe_status4, pipe_status5, pipe_status6, pipe_status7, pipe_status8;
	int a, b;	
	pid_t pid1, pid2, pid3, pid4;	
	char string[20];	
	char readbuffer[80];
	double result;

	/*Creamos las pipes*/			
	pipe_status1 = pipe(fd);
	if(pipe_status1 == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	pipe_status2 = pipe(fe);
	if(pipe_status2 == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	pipe_status3 = pipe(ff);
	if(pipe_status3 == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	pipe_status4 = pipe(fg);
	if(pipe_status4 == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	pipe_status5 = pipe(fh);
	if(pipe_status5 == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	pipe_status6 = pipe(fi);
	if(pipe_status6 == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	pipe_status7 = pipe(fj);
	if(pipe_status7 == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}

	pipe_status8 = pipe(fk);
	if(pipe_status8 == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}	

	/*Pedimos por pantalla los valores de O1 y O2*/
	printf("Introduzca 2 enteros separados por una coma: ");
	scanf("%s", string);

	/*Hacemos los fork para lanzar los procesos hijo*/	
	if((pid1 = fork())	==	-1){	
		perror("Error al hacer fork");	
		exit(EXIT_FAILURE);	
	}

	if( pid1 != 0 && ((pid2 = fork())	==	-1)){	
		perror("Error al hacer fork");	
		exit(EXIT_FAILURE);	
	}

	if(pid1 != 0 && pid2 != 0 &&((pid3 = fork())	==	-1)){	
		perror("Error al hacer fork");	
		exit(EXIT_FAILURE);	
	}


	if(pid1 != 0 && pid2 != 0 && pid3 != 0 && ((pid4 = fork())	==	-1)){	
		perror("Error al hacer fork");	
		exit(EXIT_FAILURE);	
	}
	
	/*El primer hijo realiza la potencia*/	
	if(pid1 == 0){		
		close(fd[1]);
		/*Leer los datos del pipe e imprimirlos*/	
		read(fd[0], readbuffer, sizeof(readbuffer));
		printf("PID = %d.\tHe recibido el string: %s", getpid(), readbuffer);	
		/*Separar la cadena obtenida para separar los 2 datos y pasarlos a ints*/
		a=atoi(strtok(readbuffer, ","));
		b=atoi(strtok(NULL, ","));
		/*Calcular la operación*/
		result=potencia(a, b);
		/*Crear la cadena con el resultado*/
		sprintf(readbuffer, "Resultado de la potencia: %lf", result);
		close(fh[0]);
		/*Escribir la nueva cadena en el pipe*/
		write(fh[1], readbuffer, strlen(readbuffer));
		exit(0);	
	/*El segundo hijo realiza el factorial*/	
	}else if(pid2 == 0){	
		close(fe[1]);	
		/*Leer los datos del pipe e imprimirlos*/	
		read(fe[0], readbuffer, sizeof(readbuffer));
		printf("PID = %d.\tHe recibido el string: %s\n", getpid(), readbuffer);
		/*Separar la cadena obtenida para separar los 2 datos y pasarlos a ints*/
		a=atoi(strtok(readbuffer, ","));
		b=atoi(strtok(NULL, ","));	
		/*Calcular la operación*/
		result=factorial(a, b);
		/*Crear la cadena con el resultado*/
		sprintf(readbuffer, "Resultado de O1!/O2: %lf", result);
		close(fi[0]);
		/*Escribir la nueva cadena en el pipe*/
		write(fi[1], readbuffer, strlen(readbuffer));
		exit(0);
	/*El tercer hijo realiza el número combinatorio*/ 
	}else if(pid3 == 0){	
		close(ff[1]);	
		/*Leer los datos del pipe e imprimirlos*/	
		read(ff[0], readbuffer, sizeof(readbuffer));	
		printf("PID = %d.\tHe recibido el string: %s\n", getpid(), readbuffer);
		/*Separar la cadena obtenida para separar los 2 datos y pasarlos a ints*/
		a=atoi(strtok(readbuffer, ","));
		b=atoi(strtok(NULL, ","));	
		/*Calcular la operación*/
		result=numCombinatorio(a, b);
		/*Crear la cadena con el resultado*/
		sprintf(readbuffer, "Resultado del numero combinatorio: %lf", result);
		close(fj[0]);
		/*Escribir la nueva cadena en el pipe*/
		write(fj[1], readbuffer , strlen(readbuffer));
		exit(0);
	/*El último hijo realiza la suma de valores absolutos*/
	}else if(pid4 == 0){
		close(fg[1]);	
		/*Leer los datos del pipe e imprimirlos*/	
		read(fg[0], readbuffer, sizeof(readbuffer));	
		printf("PID = %d.\tHe recibido el string: %s\n", getpid(), readbuffer);
		/*Separar la cadena obtenida para separar los 2 datos y pasarlos a ints*/
		a=atoi(strtok(readbuffer, ","));
		b=atoi(strtok(NULL, ","));
		/*Calcular la operación*/	
		result=sumaAbsoluta(a, b);
		/*Crear la cadena con el resultado*/
		sprintf(readbuffer, "Resultado de la suma de valores absolutos: %lf", result);
		close(fk[0]);
		/*Escribir la nueva cadena en el pipe*/
		write(fk[1], readbuffer , strlen(readbuffer));
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
		write(fd[1], string, strlen(string));
		write(fe[1], string, strlen(string));	
		write(ff[1], string, strlen(string));
		write(fg[1], string, strlen(string));

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