#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void main(){
	int pid;
	int i;

	for(i = 0; i < 4; i++){
		pid = fork();
		if(pid < 0){
			printf("Error al crear el fork\n");
		}else if(pid == 0){
			printf("Soy el proceso hijo %d\n", getpid());
			usleep (30000000);
			printf("Soy el proceso hijo %d y ya me toca terminar.\n", getpid());
			break;
		}else{
			usleep (5000000);
			kill(pid, SIGTERM);
		}
	}
	exit(EXIT_SUCCESS);
}

/*
Cada hijo imprime solo la primera frase porque el padre les mata antes de que pasen los 30 segundos, con lo que n pueden acabar
*/