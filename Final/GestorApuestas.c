#include "GestorApuestas.h"



double* cotizaciones;
double* pagar;
double* apuestas;
double totalApostado;
pthread_mutex_t mutex;

/**
* Funcion a la que le pasas 2 numeros y devuelve un numero aleatorio entre ambos.
*
* @param inf Int con el numero mas bajo que se quiere.
* @param sup Int con el numero mas alto que se quiere.
* @return int con un número aleatorio entre inf y sup. 
*/
void* ventanilla(){
	mensajeApuesta* mensaje;
	key = ftok(FILE_VENTANILLA_KEY, VENTANILLA_KEY);
	if(key ==(key_t) -1){
		printf("Error al obtener la clave de la cola de mensajes.");
	}

	mensaje_id = msgget(key, IPC_CREAT | 0660);
	if(mensaje_id == -1){
		printf("Error al crear la cola de mensajes de apuestas.");
	}

	while(1){
		res = msgrcv(mensaje_id, (struct msgbuf*)&mensaje, sizeof(mensajeApuesta) - sizeof(long) - sizeof(double), 0, MSG_NOERROR);
		if(res == -1){
			printf("Error al recibir el mensaje de apuesta\n");
		}

		pthread_mutex_lock(&mutex);
		pagar[mensaje->text] = cotizaciones[mensaje->type]* mensaje->apuesta;
		apuestas[mensaje->type] += mensaje->apuesta;
		totalApostado += mensaje->apuesta;
		cotizaciones[mensaje->type] = totalApostado / apuestas[mensaje->type];
		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}
void crearVentanillas(int N, pthread_t* h){
	int i;
    if(N < 1) return -1;
    for(i = 0; i < N; i++){
    	pthread_create(h[i], NULL, ventanilla, NULL);
    }
}

void esperarVentanillas(int N, pthread_t* h){
	int i;
    if(N < 1) return -1;
    for(i = 0; i < N; i++){
    	pthread_join(h[i], NULL);
    }
}


void apostador(){
	int res;
	int mensaje_id;
	key_t key;
	mensajeApuesta* mensaje;
	key = ftok(FILE_VENTANILLA_KEY, VENTANILLA_KEY);
	if(key ==(key_t) -1){
		printf("Error al obtener la clave de la cola de mensajes.");
	}

	mensaje_id = msgget(key, IPC_CREAT | 0660);
	if(mensaje_id == -1){
		printf("Error al crear la cola de mensajes de apuestas.");
	}

	mensaje = (mensajeApuesta*)malloc(sizeof(mensajeApuesta));
	if(mensaje == NULL){
		printf("Error al reservar memoria para la cola de mensajes.\n");
	}

	while(1){
		res = msgsnd(mensaje_id, (struct msgbuf*)&mensaje, sizeof(mensajeApuesta) - sizeof(long) - sizeof(double), 0, MSG_NOERROR);
		if(res == -1){
			printf("Error al recibir el mensaje de apuesta\n");
		}
		usleep(1);
	}
}


void gestor(int numCaballos, int numApostadores, int numVentanillas){
	int i;
	pthread_t *h;
	

	totalApostado = numCaballos;
	apuestas = (double*)malloc(sizeof(double)*numCaballos);
	for(i=0; i<numCaballos; i++){
		apuestas[i] = 1;
	}
	cotizaciones = (double*)malloc(sizeof(double)*numCaballos);
	for(i=0; i<numCaballos; i++){
		cotizaciones[i] = totalApostado/apuestas[i];
	}
	pagar = (double*)malloc(sizeof(double)*numApostadores);
	for(i=0; i<numApostadores; i++){
		pagar[i] = 0;
	}

	h = (pthread_t*)malloc(sizeof(pthread_t)*N);
	if(h==NULL){
		printf("Error al reservar memoria para los threads.\n");
	}
	crearVentanillas(numVentanillas, h);
	esperarVentanillas(numVentanillas, h);
}


