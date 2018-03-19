#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define NUM_HIJOS 5

void captura(){
	return;
}

void terminar(){
	exit(EXIT_SUCCESS);
}

int main(){
	int pid[NUM_HIJOS];
	int i, j;

	/*Definimos la señal para el padre*/
	if(signal(SIGUSR1, captura)== SIG_ERR){
		perror("signal");
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < NUM_HIJOS; i++){
		pid[i] = fork();
		if(pid < 0){
			if(signal(SIGUSR2, terminar)== SIG_ERR){
				perror("signal");
				exit(EXIT_FAILURE);
			}

			printf("Error al crear el examen\n");
		}else if(pid[i] == 0){
			if(i != 0){
				/*Mandamos al otro proceso hijo la señal para que acabe.*/
				kill(pid[i-1], SIGUSR2);
			}
			for(j = 0; j < 10; j++){
				printf("Soy %d y estoy trabajando\n", getpid());
				sleep (1);
			}

			kill(getppid(), SIGUSR1);

			/*Seguimos imprimiendo hasta que el otro hijo
			mande la señal*/
			while(1){
				printf("Soy %d y estoy trabajando\n", getpid());
				sleep (1);
			}
		}else{
			pause();
		}
	}

	/*Mandamos una señal al último proceso hijo para que termine*/
	kill(pid[i-1], SIGUSR2);
	/*Mirar si es necesario y cambiar por waitpid ¿?*/
	for(i = 0; i < NUM_HIJOS; i++){
		wait(NULL);
	}

	exit(EXIT_SUCCESS);
}

/*
Cada hijo imprime solo la primera frase porque el padre les mata antes de que pasen los 30 segundos, con lo que n pueden acabar
*/