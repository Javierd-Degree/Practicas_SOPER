/**
* @brief Ejercicio 6b de la Practica 2.
*
* @file Ejercicio6b.c
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
* @brief Esta funcion determina el funcionamiento que tendra el hijo tras
* recibir la senal SIGTERM. En este caso al recibir esta senal el proceso
* hijo ejecutara esta funcion que hara que imprima Soy <PID> y he recibido
* la señal SIGTERM y termine su ejecucion.
*/
void recibirSIGTERM(){
	printf("Soy %d y he recibido la señal SIGTERM\n", getpid());
	exit(EXIT_SUCCESS);
}

/**
* @brief El proceso padre crea un proceso hijo, que
* inicializa una funcion que maneja la senal SIGTERM
* (define lo que tiene que hacer el proceso hijo al
* recibir esta senal), y comienza a realizar lo mismo
* que en el ejercicio 6a, imprimir 0 1 2 3 4 y esperar
* 1 segundo y 3 segundos continuamente.
* El proceso padre tras lanzar el hijo duerme 40 segundos,
* y al pasar este tiempo lanza la senal SIGTERM al hijo y 
* realiza un wait para esperar a que el hijo acabe, tras
* lo cual acaba su ejecucion.
*
* @return int que determina si el programa se ha ejecutado o no con exito.
*/
int main(){
	int pid, counter;

	pid = fork();
	if(pid < 0){
		printf("Error al crear el proces\n");
		exit(EXIT_FAILURE);
	}if (pid == 0){
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
		if(kill(pid, SIGTERM) == -1){
			printf("Error mandando la señal.\n");
		}
	}

	wait(NULL);
	exit(EXIT_SUCCESS);
}