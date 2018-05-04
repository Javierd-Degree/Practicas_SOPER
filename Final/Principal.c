#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "Caballos.h"
#include "Semaforo.h"

/*Libera los recursos de los caballos.
Es necesario llamarla en cada uno de los procesos y en el padre.*/
void liberarCaballos(int numCaballos, int *pids, int *posTemp, int **pipes){
	int i;
	free(pids);
	free(posTemp);
	for(i = 0; i < numCaballos; i++){
		free(pipes[i]);
	}
	free(pipes);
}

void carrera(int numCaballos, int longitudCarrera){
	int *pidCaballos;
	int *posCaballos, *tiaradasCaballos;
	int i;
	int mensaje_id;
	int mem_id;
	int semid;
	int res;
	mensajeCaballo mensaje;
	int **pipesCaballos;
	char temp[100];
	key_t key, memkey;
	unsigned short  semvalor = 1;

	/* Tabla con los pid de los caballos, que usamos tambien 
	para saber si han acabado o no la carrera*/
	pidCaballos = (int *) malloc(sizeof(int)* numCaballos);

	/*Creamos e inicializamos el semáforo que controlará la entrada a la memoria compartida*/
	res = Crear_Semaforo((key_t)SEMKEY, 1,  &semid);
	if(res == -1){
		printf("Error al crear el array de semáforos\n");
	}
	res = Inicializar_Semaforo(semid, &semvalor);
	if(res == -1){
		printf("Error al inicializar el array de semáforos\n");
	}

	/*Inicializamos la memoria compartida.*/
	memkey = ftok(FILE_CABALLO_MEM_KEY, CABALLO_MEM_KEY);
	if(memkey ==(key_t) -1){
		printf("Error al obtener la clave de la memoria compartida.");
	}
	mem_id = shmget(memkey, numCaballos*sizeof(int), IPC_CREAT | SHM_W | SHM_R);


	/*Obtenemos y modificamos los datos de la memoria compartida*/
	res = Down_Semaforo(semid, 0, SEM_UNDO);
	if(res == -1){
		printf("Error al bajar el semáforo");
	}
	posCaballos = (int*)shmat(mem_id, (char*)0, 0);
	if(posCaballos == NULL) {
		printf ("Error reserve shared memory \n");
	}

	for(i = 0; i < numCaballos; i++){
		posCaballos[i] = 0;
	}

	/*Liberamos el direccionamiento virtual de la memoria compartida*/
	shmdt(posCaballos);
	res = Up_Semaforo(semid, 0, SEM_UNDO);
	if(res == -1){
		printf("Error al subir el semáforo");
	}

	/*Usamos tiaradasCaballos solo para sumar las tiradas todas a la vez.
	De lo contrario, las posiciones no tendrian sentido.*/
	tiaradasCaballos = (int *) calloc(numCaballos, sizeof(int));
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
			liberarCaballos(numCaballos, pidCaballos, tiaradasCaballos, pipesCaballos);
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

			/*Obtenemos y modificamos los datos de la memoria compartida*/
			res = Down_Semaforo(semid, 0, SEM_UNDO);
			if(res == -1){
				printf("Error al bajar el semáforo");
			}

			posCaballos = (int*)shmat(mem_id, (char*)0, 0);
			if(posCaballos == NULL) {
				printf ("Error reserve shared memory \n");
			}

			sprintf(temp, "%d", posicionCaballo(i, posCaballos, numCaballos));
			write(pipesCaballos[i][1], temp, strlen(temp)+1);

			/*Se bloquea hasta que recibe la tirada del caballo.*/
			res = msgrcv(mensaje_id, (struct msgbuf*)&mensaje, sizeof(mensaje) - sizeof(long), 0, MSG_NOERROR);
			if(res == -1){
				printf("Error al recibir mensaje de caballo.");
			}
			
			tiaradasCaballos[i] = atoi(mensaje.text);
			/*Liberamos el direccionamiento virtual de la memoria compartida*/
			shmdt(posCaballos);
			res = Up_Semaforo(semid, 0, SEM_UNDO);
			if(res == -1){
				printf("Error al subir el semáforo");
			}
		}

		/*Sumamos las tiradas de los caballos y comprobamos su posicion.*/
		for(i = 0; i < numCaballos; i++){
			if(pidCaballos[i] == META){
				/*El caballo ya ha acabado.*/
				continue;
			}

			/*Obtenemos y modificamos los datos de la memoria compartida*/
			res = Down_Semaforo(semid, 0, SEM_UNDO);
			if(res == -1){
				printf("Error al bajar el semáforo");
			}
			posCaballos = (int*)shmat(mem_id, (char*)0, 0);
			if(posCaballos == NULL) {
				printf ("Error reserve shared memory \n");
			}

			posCaballos[i] += tiaradasCaballos[i];
			tiaradasCaballos[i] = 0;

			if(posCaballos[i] >= longitudCarrera){
				/*Marcamos el pid como -1, porque ya hemos acabado, 
				y modificamos nuestra posicion indicando el puesto en el
				que hemos quedado.*/
				posCaballos[i] = carreraAcabada(pidCaballos, numCaballos) + 1;
				pidCaballos[i] = META;
			}
			/*Liberamos el direccionamiento virtual de la memoria compartida*/
			shmdt(posCaballos);
			res = Up_Semaforo(semid, 0, SEM_UNDO);
			if(res == -1){
				printf("Error al subir el semáforo");
			}
		}


		if(carreraAcabada(pidCaballos, numCaballos) == numCaballos){
			break;
		}
	}

	printf("Carrera acabada. Los resultados son:\n");

	/*Obtenemos y modificamos los datos de la memoria compartida*/
	res = Down_Semaforo(semid, 0, SEM_UNDO);
	if(res == -1){
		printf("Error al bajar el semáforo");
	}
	posCaballos = (int*)shmat(mem_id, (char*)0, 0);
	if(posCaballos == NULL) {
		printf ("Error reserve shared memory \n");
	}

	for(i = 0; i < numCaballos; i++){
		printf("%d ", posCaballos[i]);
	}
	/*Liberamos el direccionamiento virtual de la memoria compartida*/
	shmdt(posCaballos);
	res = Up_Semaforo(semid, 0, SEM_UNDO);
	if(res == -1){
		printf("Error al subir el semáforo");
	}

	for(i = 0; i < numCaballos; i ++){
		wait(NULL);
	}

	liberarCaballos(numCaballos, pidCaballos, tiaradasCaballos, pipesCaballos);
	/*Eliminamos la memoria compartida*/
	shmctl (mem_id, IPC_RMID, (struct shmid_ds *)NULL);
	/*Liberamos el array de semáforos*/
	res = Borrar_Semaforo(semid);
	if(res == -1){
		printf("Error al liberar el semáforo");
	}
	/*Liberamos la cola de mensajes*/
	msgctl(mensaje_id, IPC_RMID, (struct msqid_ds*) NULL);
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