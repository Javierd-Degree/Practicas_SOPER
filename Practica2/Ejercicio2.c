/**
* @brief Ejercicio 2 de la Practica 2.
*
* @file Ejercicio2.c
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/**
* @brief El proceso padre crea cuatro procesos hijos, que imprimen
* "Soy el proceso hijo <PID>", duermen treinta segundos, y luego 
* imprimen "Soy el proceso hijo <PID> y ya me toca terminar.".
* Una vez el proceso padre crea cada uno de los hijos, espera 30
* segundos y mata al hijo enviando la senal SIGTERM.
* De esta forma, es imposible que los hijos impriman el segundo 
* mensaje, pues el padre los habra matado antes.
*
* @return int que determina si el programa se ha ejecutado o no con exito.
*/
int main(){
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
RESPUESTA:
Cada hijo imprime solo la primera frase porque el padre les mata antes de que pasen los 30 segundos, con lo que n pueden acabar
*/