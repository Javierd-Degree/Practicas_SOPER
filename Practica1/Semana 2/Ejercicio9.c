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
	return abs(a) + abs(b);
}
	
int	main(void){	
	int	fd[2], nbytes, pipe_status;	
	pid_t pid1, pid2, pid3, pid4;	
	char string[] = "Hola a todos!\n";	
	char readbuffer[80];	
		
	pipe_status = pipe(fd);

	if(pipe_status == -1){	
		perror("Error creando la tuberia\n");	
		exit(EXIT_FAILURE);	
	}	
		
	if((pid1 = fork())	==	-1){	
		perror("Erro al hacer fork");	
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
		
	if(pid1 == 0){		
		close(fd[1]);	
		nbytes = read(fd[0], readbuffer, sizeof(readbuffer));	
		printf("PID = %d.\tHe recibido el string: %s\n", getpid(), readbuffer);	
		exit(0);	
	}else if(pid2 == 0){	
		close(fd[1]);	
		nbytes = read(fd[0], readbuffer, sizeof(readbuffer));	
		printf("PID = %d.\tHe recibido el string: %s\n", getpid(), readbuffer);	
		exit(0);
	}else if(pid3 == 0){	
		close(fd[1]);	
		nbytes = read(fd[0], readbuffer, sizeof(readbuffer));	
		printf("PID = %d.\tHe recibido el string: %s\n", getpid(), readbuffer);	
		exit(0);
	}else if(pid4 == 0){
		close(fd[1]);	
		nbytes = read(fd[0], readbuffer, sizeof(readbuffer));	
		printf("PID = %d.\tHe recibido el string: %s\n", getpid(), readbuffer);	
		exit(0);	
	}else{	
		/* Cierre del descriptor de salida en el padre*/	
		close(fd[0]);	
		/*Leer algo de la tubería... el saludo! */
		write(fd[1], string, strlen(string));	
	}

	return 0;	
}