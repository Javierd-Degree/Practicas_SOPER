#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define NUM_PROC 5
int main (void){
	int pid, counter;
	sigset_t set, oset;

	pid = fork();
	if (pid == 0){
		/*Inicializamos el array como uno vacío, no se bloquea ninguna señal*/
		sigemptyset(&set);
		sigemptyset(&oset);
		sigaddset(&set, SIGUSR2);
		sigaddset(&oset, SIGUSR1);
		sigaddset(&oset, SIGALRM);

		alarm(40);
		while(1){
			/*Bloqueamos las señales del conjunto union de ambos*/
			sigprocmask(SIG_BLOCK, &set,&oset);
			for (counter = 0; counter < NUM_PROC; counter++){
				printf("%d\n", counter);
				sleep(1);
			}
			/*Desloqueamos las señales del conjunto oset*/
			sigprocmask(SIG_UNBLOCK, &set, &oset);
			sleep(3);
		}
	}

	wait(NULL);
	exit(EXIT_SUCCESS);
}

/**
* Cuando el hijo recibe la señal, esta señal se queda bloqueada esperando a que sea dsbloqueada. Una vez acaba el bucle for,
* antes del sleep de tres sgundos, se desbloquea dicha señal, y por tanto, como el hijo no tiene un manejador de la señal 
* SIGALRM, realiza la accion por defecto, que es terminar el proceso, SIG_DFL.
*/