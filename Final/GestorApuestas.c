/**
* @brief Gestor de apuestas del Proyecto Final.
*
* @file GestorApuestas.c
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/

#include "GestorApuestas.h"
#include <errno.h>


pthread_t *h;
pthread_mutex_t mutex;

recursosGestor* recursos;

/**
* @brief Funcion que inicializa los recursos que empleara el monitor y el gestor, entre ellos esta
* la memoria compartida, el array de semaforos para la memoria compartida y el report y la cola de mensajes.
* @param recs Estructura en la que se guardan los ids de la memoria compartida, el array de semaforos y el de la cola de mensajes.
* @return int indicando si la funcion se ha ejecutado con éxito.
*/
int inicializaRecursosGestor(recursosGestor* recs){
	int semid;
	int memid;
	int mensaje_id;
	int res;
	key_t semkey;
	key_t memkey;
	key_t msgkey;
	unsigned short semvalor[3] = {1, 1, 0};

	/*Creamos e inicializamos el semáforo que controlará la entrada a la memoria compartida*/
	semkey = ftok(FILE_MEM_COMP_GESTOR_KEY, SEMKEYGESTOR);
	if(semkey ==(key_t) -1){
		printf("Error al obtener la clave del semaforo.");
		return -1;
	}
	res = Crear_Semaforo(semkey, 3,  &semid);
	if(res == -1){
		printf("Error al crear el array de semáforos\n");
		return -1;
	}
	res = Inicializar_Semaforo(semid, semvalor);
	if(res == -1){
		printf("Error al inicializar el array de semáforos. %s\n", strerror(errno));
		return -1;
	}
	/*Inicializamos la memoria compartida.*/
	memkey = ftok(FILE_MEM_COMP_GESTOR_KEY, MEM_COMP_GESTOR_KEY);
	if(memkey ==(key_t) -1){
		printf("Error al obtener la clave de la memoria compartida.");
		return -1;
	}

	memid = shmget(memkey, sizeof(memCompartida), IPC_CREAT | SHM_W | SHM_R);
	if(memid == -1){
		printf("Error al obtener el id de la memoria compartida del gestor.\n %s",strerror(errno));
		return -1;
	}

	msgkey = ftok(FILE_VENTANILLA_KEY, VENTANILLA_KEY);
	if(msgkey ==(key_t) -1){
		printf("Error al obtener la clave de la cola de mensajes.");
	}

	mensaje_id = msgget(msgkey, IPC_CREAT | 0660);
	if(mensaje_id == -1){
		printf("Error al crear la cola de mensajes de apuestas.");
	}

	recs->semid = semid;
	recs->memid = memid;
	recs->mensaje_id = mensaje_id;
	return 0;
}



/**
* @brief Funcion que libera los recursos que empleara el monitor y el gestor, entre ellos esta
* la memoria compartida, el array de semaforos para la memoria compartida y el report y la cola de mensajes.
* @param recs Estructura en la que se guardan los ids de la memoria compartida, el array de semaforos y el de la cola de mensajes.
* @return int indicando si la funcion se ha ejecutado con éxito.
*/
int liberarRecursosGestor(recursosGestor* recs){
	int res;

	res = Borrar_Semaforo(recs->semid);
	if(res == -1){
		printf("Error al borrar el array de semáforos\n");
		return -1;
	}
	
	res = shmctl(recs->memid, IPC_RMID, (struct shmid_ds *)NULL);
	if(res == -1){
		printf("Error al eliminar la memoria compartida del gestor.\n");
		return -1;
	}


	res = msgctl(recs->mensaje_id, IPC_RMID, (struct msqid_ds*)NULL);
	if(res == -1){
		printf("Error al liberar la cola de mensajes.\n");
		return -1;
	}
	return 0;
}


/**
* @brief Funcion se encarga de recibir los mensajes de apuesta de la cola de mensajes y procesarlos actualizando las cotizaciones de cada
* caballo, lo que se pafar a cada apostador si gana..., e imprime en el report los datos de las apuestas.
* @param resGestor Estructura en la que se guardan los ids de la memoria compartida, el array de semaforos y el de la cola de mensajes.
* @return void* Debe ser void* pues la empleara un thread, pero no se emplea para nada asi que es NULL.
*/
void* ventanilla(void* resGestor){
	mensajeApuesta mensaje;
	memCompartida* mem;
	int apostador;
	int res;
	int i;
	char string[200];
	int numVentanilla;
	recursosGestor* recs;

	recs = (recursosGestor*)resGestor;
	numVentanilla = recs->numVentanilla;
	res = Up_Semaforo(recs->semid, 2, SEM_UNDO);
	if(res == -1){
		printf("Error al subir el semafono del id de ventanilla.\n");
	}

	while(1){
		res = msgrcv(recs->mensaje_id, (struct msgbuf*)&mensaje, sizeof(mensajeApuesta) - sizeof(long), 0, 0);
		if(res == -1){
			printf("Error al recibir el mensaje de apuesta\n");
		}
		sprintf(string, "%s ha hecho una apuesta de %lf al caballo %ld en la ventanilla %d\n", mensaje.text, mensaje.apuesta, mensaje.type, numVentanilla);
		monitorImprimeReport(string, recs->semid);

		pthread_mutex_lock(&mutex);
		Down_Semaforo(recs->semid, 0, SEM_UNDO);
		mem = (memCompartida*)shmat(recs->memid, (char*)0, 0);
		if(mem == NULL) {
			printf ("Error reserve shared memory \n");
		}
		sscanf(mensaje.text, "Apostador-%d", &apostador);
		mem->apostadorCaballo[apostador] = (int) mensaje.type;
		mem->cantidadApostada[apostador] = mensaje.apuesta;
		mem->pagar[apostador] = mem->cotizaciones[mensaje.type]* mensaje.apuesta;
		mem->apuestas[mensaje.type] += mensaje.apuesta;
		mem->totalApostado += mensaje.apuesta;
		for(i=0; i<10; i++){
			mem->cotizaciones[i] = mem->totalApostado / mem->apuestas[i];
		}
		Up_Semaforo(recs->semid, 0, SEM_UNDO);
		pthread_mutex_unlock(&mutex);
		shmdt(mem);
	}

	return NULL;
}

/**
* @brief Funcion que crea los threads que ejecutaran las distintas ventanillas de apuestas.
* @param N int que indica el numero de ventanillas a crear.
* @param h, array de los threads a crear.
* @param recs Estructura en la que se guardan los ids de la memoria compartida, el array de semaforos y el de la cola de mensajes.
*/
void crearVentanillas(int N, pthread_t* h, recursosGestor* recs){
	int i;
	int res;

	recs->numVentanilla = 1;
	
    for(i = 0; i < N; i++){
    	pthread_create(&h[i], NULL, ventanilla, (void*)recs);
    	res = Down_Semaforo(recs->semid, 2, SEM_UNDO);
		if(res == -1){
			printf("Error al bajar el semafono del id de ventanilla.\n");
		}
		recs->numVentanilla++;
    }
}


/**
* @brief Funcion que espera a que se finalice la ejecucion de los threads.
* @param N int que indica el numero de ventanillas a crear.
* @param h, array de los threads creados.
*/
void esperarVentanillas(int N, pthread_t* h){
	int i;
    for(i = 0; i < N; i++){
    	pthread_join(h[i], NULL);
    }
}

/**
* @brief Funcion que ejecutara el proceso apostador, genera una apuesta de valor aleatorio para un caballo aleatorio
* cada segundo y la sube a la cola de mensajes para que las ventanillas la procesen.
* @param numCaballos int que indica el numero de caballos que participan en la carrera.
* @param numApostadores int que indica el numero de apostadores para la carrera.
* @param maxApuesta int que indica el dinero del que disponen los apostadores.
* @param recs Estructura en la que se guardan los ids de la memoria compartida, el array de semaforos y el de la cola de mensajes.
*/
void apostador(int numCaballos, int numApostadores, int maxApuesta, recursosGestor* recs){
	int res;
	int i;
	mensajeApuesta mensaje;


	for(i = 0; i<numApostadores; i++){
		mensaje.type = aleatNum(1, numCaballos);
		mensaje.apuesta = ((double)aleatNum(100, 100*maxApuesta))/100;
		sprintf(mensaje.text, "Apostador-%d", i);
		//printf("%s va a hacer una apuesta de %lf al caballo %ld\n", mensaje.text, mensaje.apuesta, mensaje.type);
		res = msgsnd(recs->mensaje_id, (struct msgbuf*)&mensaje, sizeof(mensajeApuesta) - sizeof(long), IPC_NOWAIT);
		if(res == -1){
			printf("Error al enviar el mensaje de apuesta\n %s\n", strerror(errno));
		}
		usleep(1000000);
	}
}

/**
* @brief Funcion de control de la senal SIGUSR1 pra el proceso gestor para que este salga con exito.
*/
void salida(){
	free(h);
	exit(EXIT_SUCCESS);
}

/**
* @brief Funcion que ejecutara el proceso gestor, inicializa todos los datos de la memoria compartida a sus datos iniciales, crea
* los threds para las ventanillas y espera a que se ejecuten hasta recibir la senal SIGUSR1.
* @param numCaballos int que indica el numero de caballos que participan en la carrera.
* @param numApostadores int que indica el numero de apostadores para la carrera.
* @param numVentanillas int que indica el numero de threads a crear para las ventanillas.
* @param recs Estructura en la que se guardan los ids de la memoria compartida, el array de semaforos y el de la cola de mensajes.
*/
void gestor(int numCaballos, int numApostadores, int numVentanillas, recursosGestor* recs){
	int i;
	int res;
	memCompartida* mem;


	recursos = recs;

	if(signal(SIGUSR1, salida)== SIG_ERR){
		perror("signal");
		exit(EXIT_FAILURE);
	}
	
	res = Down_Semaforo(recs->semid, 0, SEM_UNDO);
	if(res == -1){
		printf("Error en el down del semaforo del gestor.\n");
	}
	mem = shmat(recs->memid, (char*)0, 0);
	if(mem == NULL) {
		printf ("Error reserve shared memory \n");
	}

	mem->totalApostado = numCaballos;
	
	for(i=0; i<10; i++){
		mem->apuestas[i] = 1;
	}
	
	for(i=0; i<10; i++){
		mem->cotizaciones[i] = mem->totalApostado/mem->apuestas[i];
	}

	for(i=0; i<100; i++){
		mem->pagar[i] = 0;
		mem->apostadorCaballo[i] = -1;
		mem->cantidadApostada[i] = 0;
	}
	
	shmdt(mem);
	
	res = Up_Semaforo(recs->semid, 0, SEM_UNDO);
	if(res == -1){
		printf("Error en el up del semaforo del gestor.\n");
	}

	h = (pthread_t*)malloc(sizeof(pthread_t)*numVentanillas);
	if(h==NULL){
		printf("Error al reservar memoria para los threads.\n");
	}

	crearVentanillas(numVentanillas, h, recs);
	esperarVentanillas(numVentanillas, h);
}


