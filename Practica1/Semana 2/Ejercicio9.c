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
	int	fd[2], fe[2], ff[2], fg[2], fh[2], fi[2], fj[2], fk[2], nbytes;
	int pipe_status1, pipe_status2, pipe_status3, pipe_status4, pipe_status5, pipe_status6, pipe_status7, pipe_status8;
	int a1, b1, a2, b2, a3, b3, a4, b4;	
	pid_t pid1, pid2, pid3, pid4;	
	char string[] = "2,3";	
	char readbuffer[80], readbuffer1[80], readbuffer2[80], readbuffer3[80], readbuffer4[80];
	double result;			
		
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
		
	if(pid1 == 0){		
		close(fd[1]);	
		nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
		printf("PID = %d.\tHe recibido el string: %s\n", getpid(), readbuffer);	
		a1=atoi(strtok(readbuffer, ","));
		b1=atoi(strtok(NULL, ","));
		result=potencia(a1, b1);
		sprintf(readbuffer, "Resultado de la potencia: %lf\n", result);
		close(fh[0]);
		write(fh[1], readbuffer, strlen(readbuffer));
		exit(0);	
	}else if(pid2 == 0){	
		close(fe[1]);	
		nbytes = read(fe[0], readbuffer, sizeof(readbuffer));
		printf("PID = %d.\tHe recibido el string: %s\n", getpid(), readbuffer);
		a2=atoi(strtok(readbuffer, ","));
		b2=atoi(strtok(NULL, ","));	
		result=factorial(a2, b2);
		sprintf(readbuffer, "Resultado de O1!/O2: %lf\n", result);
		close(fi[0]);
		write(fi[1], readbuffer, strlen(readbuffer));
		exit(0);
	}else if(pid3 == 0){	
		close(ff[1]);	
		nbytes = read(ff[0], readbuffer, sizeof(readbuffer));	
		printf("PID = %d.\tHe recibido el string: %s\n", getpid(), readbuffer);
		a3=atoi(strtok(readbuffer, ","));
		b3=atoi(strtok(NULL, ","));	
		result=numCombinatorio(a3, b3);
		sprintf(readbuffer, "Resultado del numero combinatorio: %lf\n", result);
		close(fj[0]);
		write(fj[1], readbuffer , strlen(readbuffer));
		exit(0);
	}else if(pid4 == 0){
		close(fg[1]);	
		nbytes = read(fg[0], readbuffer, sizeof(readbuffer));	
		printf("PID = %d.\tHe recibido el string: %s\n", getpid(), readbuffer);
		a4=atoi(strtok(readbuffer, ","));
		b4=atoi(strtok(NULL, ","));	
		result=sumaAbsoluta(a4, b4);
		sprintf(readbuffer, "Resultado de la suma de valores absolutos: %lf\n", result);
		close(fj[0]);
		write(fj[1], readbuffer , strlen(readbuffer));
		exit(0);	
	}else{	
		/* Cierre del descriptor de salida en el padre*/	
		close(fd[0]);
		close(fe[0]);
		close(ff[0]);
		close(fg[0]);
		close(fh[1]);
		close(fi[1]);
		close(fj[1]);
		close(fk[1]);	
		/*Leer algo de la tubería... el saludo! */
		write(fd[1], string, strlen(string));
		write(fe[1], string, strlen(string));	
		write(ff[1], string, strlen(string));
		write(fg[1], string, strlen(string));


		nbytes = read(fh[0], readbuffer1, sizeof(readbuffer1));	
		printf("\n%s\n", readbuffer1);

		nbytes = read(fi[0], readbuffer2, sizeof(readbuffer2));
		printf("%s\n", readbuffer2);

		nbytes = read(fj[0], readbuffer3, sizeof(readbuffer3));
		printf("%s\n", readbuffer3);

		nbytes = read(fk[0], readbuffer4, sizeof(readbuffer4));	
		printf("%s\n", readbuffer4);

	}

	return 0;	
}