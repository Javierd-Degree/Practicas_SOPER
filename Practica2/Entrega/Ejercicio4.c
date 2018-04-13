/**
* @brief Ejercicio 4 de la Practica 2.
*
* @file Ejercicio4.c
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define NUM_HIJOS 5 /*!< Numero de hijos que crea el proceso padre*/

/**
* @brief Funcion que ejecuta el proceso padre tras recibir
* la senal SIGUSR1. De esta forma, nos aseguramos que el
* proceso salga correctamente del pause() y continue su
* ejecucion.
*/
void captura(){
	return;
}

/**
* @brief Funcion que ejecutan los procesos hijos tras recibir la
* senal SIGUSR2, para terminar asi su ejecucion.
*/
void terminar(){
	exit(EXIT_SUCCESS);
}

/**
* @brief El proceso padre crea un proceso hijo, que
* imprime 10 veces "Soy <PID> y estoy trabajando", esperando 
* un segundo entre cada vez. Una vez el proceso hijo ha impreso
* el texto las 10 veces, manda la señal SIGUSR1 al padre, que sale
* del pause(), y crea otro hijo, de forma que es este nuevo hijo
* el que manda una senal SIGUSR2 al hijo anterior para que este se
* termine a el mismo.
* Asi, el padre acaba creando NUM_HIJOS procesos hijos, el mismo 
* mata al ultimo de los hijos, y se asegura de esperar a todos.
*
* @return int que determina si el programa se ha ejecutado o no con exito.
*/
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
			exit(EXIT_FAILURE);
			printf("Error al crear el proceso.\n");
		}else if(pid[i] == 0){
			if(signal(SIGUSR2, terminar)== SIG_ERR){
				perror("signal");
				exit(EXIT_FAILURE);
			}

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
	for(i = 0; i < NUM_HIJOS; i++){
		wait(NULL);
	}

	exit(EXIT_SUCCESS);
}