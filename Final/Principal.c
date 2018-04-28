#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "Caballos.h"

/*Libera los recursos de los caballos.
Es necesario llamarla en cada uno de los procesos y en el padre.*/
void liberarCaballos(int numCaballos, int *pids, int *pos, int *posTemp, int **pipes){
	int i;
	free(pids);
	free(pos);
	free(posTemp);
	for(i = 0; i < numCaballos; i++){
		free(pipes[i]);
	}
	free(pipes);
}

void carrera(int numCaballos, int longitudCarrera){
	int *pidCaballos;
	int *posCaballos, *posTempCaballos;
	int i;
	int mensaje_id;
	int res;
	mensajeCaballo mensaje;
	int **pipesCaballos;
	char temp[100];
	key_t key;

	/* Tabla con los pid de los caballos, que usamos tambien 
	para saber si han acabado o no la carrera*/
	pidCaballos = (int *) malloc(sizeof(int)* numCaballos);

	/*TODO posCaballos deberia ser memoria compartda, pues es lo que usamos
	desde el monitor tambien.*/
	posCaballos = (int *) calloc(numCaballos, sizeof(int));

	/*TODO  usamos posTempCaballos solo para sumar las tiradas todas a la vez.
	De lo contrario, las posicionesn no tendrian sentido.*/
	posTempCaballos = (int *) calloc(numCaballos, sizeof(int));
	if(pidCaballos == NULL || posCaballos == NULL){
		printf("Error al reservar memoria.\n");
		exit(EXIT_FAILURE);
	}

	/* Inicializamos los pipes de los caballos*/
	pipesCaballos = (int **)malloc(sizeof(int*)*numCaballos);
	if(pipesCaballos == NULL){
		printf("Error al reservar memoria.\n");
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < numCaballos; i++){
		pipesCaballos[i] = (int*)malloc(sizeof(int)* 2);
		if(pipesCaballos[i] == NULL){
			printf("Error al reservar memoria.\n");
			exit(EXIT_FAILURE);
		}

		if(pipe(pipesCaballos[i]) == -1){
			printf("Error al inicializar los pipes.\n");
			exit(EXIT_FAILURE);
		}
	}

	/*Inicializamos la cola de mensajes.*/
	key = ftok(FILE_CABALLO_KEY, CABALLO_KEY);
	if(key ==(key_t) -1){
		printf("Error al obtener la clave de la cola de mensajes.");
	}
	mensaje_id = msgget(key, IPC_CREAT | 0660);
	if(mensaje_id == -1){
		printf("Error al crear la cola de mensajes.");
	}

	/*Creamos los procesos hijo.*/
	for(i = 0; i < numCaballos; i++){
		pidCaballos[i] = fork();
		if(pidCaballos[i] == -1){
			printf("Error al hacer fork.\n");
		}else if(pidCaballos[i] == 0){
			caballo(i+1, pipesCaballos[i], longitudCarrera);
			liberarCaballos(numCaballos, pidCaballos, posCaballos, posTempCaballos, pipesCaballos);
			exit(EXIT_SUCCESS);
		}else{
			printf("ID Caballo %d: %d\n", i+1, pidCaballos[i]);
			close(pipesCaballos[i][0]);
		}
	}

	/*La carrera tiene lugar.*/
	while(1){
		/*Hacemos las tiradas de cada uno de los caballos, pero
		no las sumamos.*/
		for(i = 0; i < numCaballos; i++){
			if(pidCaballos[i] == META){
				/*El caballo ya ha acabado.*/
				continue;
			}

			sprintf(temp, "%d", posicionCaballo(i, posCaballos, numCaballos));
			write(pipesCaballos[i][1], temp, strlen(temp)+1);

			/*Se bloquea hasta que recibe la tirada del caballo.*/
			res = msgrcv(mensaje_id, (struct msgbuf*)&mensaje, sizeof(mensaje) - sizeof(long), 0, MSG_NOERROR);
			if(res == -1){
				printf("Error al recibir mensaje de caballo.");
			}
			
			posTempCaballos[i] = atoi(mensaje.text);
		}

		/*Sumamos las tiradas de los caballos y comprobamos su posicion.*/
		for(i = 0; i < numCaballos; i++){
			if(pidCaballos[i] == META){
				/*El caballo ya ha acabado.*/
				continue;
			}
			posCaballos[i] += posTempCaballos[i];
			posTempCaballos[i] = 0;

			if(posCaballos[i] >= longitudCarrera){
				/*Marcamos el pid como -1, porque ya hemos acabado, 
				y modificamos nuestra posicion indicando el puesto en el
				que hemos quedado.*/
				posCaballos[i] = carreraAcabada(pidCaballos, numCaballos) + 1;
				pidCaballos[i] = META;
			}
		}

		if(carreraAcabada(pidCaballos, numCaballos) == numCaballos){
			break;
		}
	}

	printf("Carrera acabada. Los resultados son:\n");
	for(i = 0; i < numCaballos; i++){
		printf("%d ", posCaballos[i]);
	}

	for(i = 0; i < numCaballos; i ++){
		wait(NULL);
	}

	liberarCaballos(numCaballos, pidCaballos, posCaballos, posTempCaballos, pipesCaballos);
}

int main(){
	int numCaballos;
	int longitudCarrera;


	printf("Introduce la longitud de carrera: ");
	scanf("%d", &longitudCarrera);

	do{
		printf("Introduce el numero de caballos: ");
		scanf("%d", &numCaballos);
	}while(numCaballos > 10);

	carrera(numCaballos, longitudCarrera);

	exit(EXIT_SUCCESS);
}