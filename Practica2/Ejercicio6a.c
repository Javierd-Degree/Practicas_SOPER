/**
* @brief Ejercicio 6a de la Practica 2.
*
* @file Ejercicio6a.c
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define NUM_PROC 5

/**
* @brief El proceso padre crea un proceso hijo, que
* inicializa una mascara vacia e introduce en ella las 
* senales SIGUSR1, SIGUSR2, SIGUSR3 y lanza un alarm
* que mandara una señal a los 40 segundos Tras esto entra en un
* while(1) y bloquea las señales para que si le llegan
* durante la cuenta que realizara a continuacion, no se
* pare esta y comienza a imprimir 0 1 2 3 4 y a
* esperar 1 segundo y 3 segundos continuamente, pero
* antes de esperar los 3 segundos, desbloquea las senales
* de modo que si ha recibido la senal del alarm, se para
* su ejecucion tras lo cual acaba tambien el proceso padre
* que habia hecho un wait para esperar al hijo.
*
* @return int que determina si el programa se ha ejecutado o no con exito.
*/
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