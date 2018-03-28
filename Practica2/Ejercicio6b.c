#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define NUM_PROC 5

void recibirSIGTERM(){
	printf("Soy %d y he recibido la señal SIGTERM\n", getpid());
	exit(EXIT_SUCCESS);
}

int main (void){
	int pid, counter;

	pid = fork();
	if (pid == 0){
		/*Inicializamos la funcion de manejo*/
		if(signal(SIGTERM, recibirSIGTERM)== SIG_ERR){
			perror("signal");
			exit(EXIT_FAILURE);
		}

		while(1){
			for (counter = 0; counter < NUM_PROC; counter++){
				printf("%d\n", counter);
				sleep(1);
			}
			sleep(3);
		}
	}else{
		sleep(40);
		kill(SIGTERM, pid);
	}

	wait(NULL);
	exit(EXIT_SUCCESS);
}