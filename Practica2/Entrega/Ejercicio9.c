/**
* @brief Ejercicio 9 de la Practica 2.
*
* @file Ejercicio9.c
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include "Ejercicio8.h"

#define NCAJAS 4
/*Id del array de semaforos*/
#define SEMKEY 75798
#define N_SEMAFOROS (NCAJAS + 1)
#define MAX_DINERO 1000

int semid;
int cuentaGlobal = 0;

/**
* Funcion a la que le pasas 2 numeros y devuelve un numero aleatorio entre ambos.
*
* @param inf Int con el numero mas bajo que se quiere.
* @param sup Int con el numero mas alto que se quiere.
* @return int con un número aleatorio entre inf y sup. 
*/
int aleat_num(int inf, int sup){
    if(sup < inf) return ERROR;
    if(sup == inf) return sup;
    /*Cuando nuestro intervalo es mayor que RAND_MAX, groups será 0, y no funcionará*/
    if(sup-inf+1 > RAND_MAX) return ERROR;
    int random;
    int range = sup-inf+1;
    int groups = RAND_MAX/range;
    int new_lim = groups * range;

    do{
    	random = rand();
    }while(random >= new_lim);

    return (random/groups) + inf;
}


/**
* @brief Funcion que recibe una string con el nombre de un fichero, lo abre
* e imprime en el 50 numero aleatorios entre el 0 y el 300 usando la funcion
* aleat_num creada antes.
*
* @param nombre Cadena de caracteres con el nombre del fichero.
* @return int que determina si la funcion se ha ejecutado o no con exito.
*/
int rellenar_fichero(char *nombre){
	FILE *f = NULL;
	int i;

	if(nombre == NULL){
		printf("Nombre null\n");
		return ERROR;
	}

	f = fopen(nombre, "w");
	if(f == NULL){
		printf("Error al abrir el archivo\n");
		return ERROR;
	}

	for(i = 0; i < 50; i++){
		fprintf(f, "%d\n", aleat_num(0, 300));
	}

	fclose(f);
	return OK;
}

/*Delvuelve la cantidad total guardada en el fichero.*/
/*Modo puede ser 0 para sumar, 1 para restar, 2 para vaciar la caja
En el caso en el que vaciamos la caja, valor se ignora.*/
/**
* @brief Funcion que recibe una caja, un modo y un valor y modifica la caja
* bajando el semaforo antes de modificar el archivo y subiendolo al acabar,
* de modo que no haya otro proceso que pueda modificar el archivo a la vez
* que este. Si el modo es 0 suma el valor a lo que ya hay en la caja, si el
* modo es 1 restas el valor a lo que hay en la caja, y si el modo es 2 se
* vacia completamente la caja.
*
* @param caja Int con la caja que se va a modificar.
* @param valor Int con el valor con el que se va a modificar la caja.
* @param modo Int con el modo del que se quiere modificar la caja.
* @return int con el valor final de la caja.
*/
int mod_caja(int caja, int valor, int modo){
	char nombre[512];
	FILE *f = NULL;
	int temp = 0;
	int result;
	int ret;
	
	sprintf(nombre, "%d.txt", caja);
	ret = Down_Semaforo(semid, caja, SEM_UNDO);
	if(ret == ERROR){
		printf("Error al bajar semaforo 1, %d\n", caja);
	}

	/*Bajamos el semaforo para asegurarnos de que no leemos y escribimos
	a la vez desde el padre y el hijo.*/
	/*Cogemos el valor actual, o creamos el archivo si no existe*/
	f = fopen(nombre, "r");
	if(f == NULL){
		printf("El archivo %s no existe, lo creamos\n", nombre);
		f = fopen(nombre, "w");
	}else{
		if(fscanf(f, "%d", &temp) != 1){
			printf("\tError en el scanf caja %d. No carga temp.\n", caja);
			exit(-1);
		}
		fclose(f);
		f = fopen(nombre, "w");
		//printf("El archivo %s existe y tiene %d euros\n", nombre, temp);
	}

	if(modo == 0){
		fprintf(f, "%d", temp + valor);
		result = temp + valor;
	}else if(modo == 1 && temp >= MAX_DINERO){
		/*Nos aseguramos aqui de que haya mas de 1000 euros, pues 
		puede darse el caso de que el hijo tenga guardado en la variable 
		temp el dinero de antes de que se actualizase su caja, y por tanto,
		vuelva a llamar al padre.*/
		printf("Soy el padre y acabo de quitarle 900€ a la caja %d, que tenia %d€\n", caja, temp);
		fprintf(f, "%d", temp - valor);
		result = valor;
	}else if(modo == 1 && temp < MAX_DINERO){
		fprintf(f, "%d", temp);
		result = 0;
	}else if(modo == 2){
		fprintf(f, "0");
		result = temp;
	}
	
	fclose(f);

	ret = Up_Semaforo(semid, caja, SEM_UNDO);
	if(ret == ERROR){
		printf("Error al subir semaforo 1, %d\n", caja);
	}

	return result;
}

/*Dado que antes de mandar la señal que llama a esta funcion, el
proceso hijo hace un down del semaforo, ningun otro proceso podra
lanzar dicha señal, y por tanto no es necesario usar una mascara 
se señales para bloquearlas.*/
/**
* @brief Funcion de manejo de la senal USR1 que utiliza el proceso padre
* para saber que tiene que quitar 900 euros al hijo. Esta funcion llama
* a mod_caja y sube el ultimo semaforo que es el que se usa para manejar
* el archivo en el que se controla que proceso es el que ha llamado.
*/
void retirarParte(){
	FILE *f;
	int i;
	int ret;
	f = fopen("procesoEspera.txt", "r");
	if(f == NULL){
		return;
	}
	
	if(fscanf(f, "%d", &i) != 1 || i < 0 || i >= NCAJAS){
		printf("Error al intentar saber que proceso ha mandado la señal. %d\n", i);
	}else{
		cuentaGlobal += mod_caja(i, 900, 1);
	}

	fclose(f);
	ret = Up_Semaforo(semid, N_SEMAFOROS - 1, SEM_UNDO);
	if(ret == ERROR){
		printf("Error al subir semaforo 2 para retirar todo.\n");
		return;
	}
}

/*Dado que antes de mandar la señal que llama a esta funcion, el
proceso hijo hace un down del semaforo, ningun otro proceso podra
lanzar dicha señal, y por tanto no es necesario usar una mascara 
se señales para bloquearlas.*/
/**
* @brief Funcion de manejo de la senal USR1 que utiliza el proceso padre
* para saber que tiene que quitar todo el dinero al hijo. Esta funcion llama
* a mod_caja y sube el ultimo semaforo que es el que se usa para manejar
* el archivo en el que se controla que proceso es el que ha llamado.
*/
void retirarTotal(){
	int ret;
	int i = -1;
	FILE *f = NULL;

	f = fopen("procesoEspera.txt", "r");
	if(f == NULL){
		return;
	}
	
	if(fscanf(f, "%d", &i) != 1 || i < 0 || i >= NCAJAS){
		printf("Error al intentar saber que proceso ha mandado la señal.\n");
	}else{
		cuentaGlobal += mod_caja(i, 0, 2);
		printf("Soy el padre y acabo de quitarle todo el dinero a la caja %d\n", i);
	}

	fclose(f);
	
	ret = Up_Semaforo(semid, N_SEMAFOROS - 1, SEM_UNDO);
	if(ret == ERROR){
		printf("Error al subir semaforo 2 para retirar todo.\n");
		return;
	}
}

/**
* @brief Main que crea e inicializa un array de semaforos a 1, crea los
* archivos para todas las cajas llamando a la funcion rellenar_fichero,
* y hace los forks, Tras esto los hijos escanean el dinero que hay en su
* caja y van llamando a mod_caja hasta que tienen 1000 euros o han
* terminado el bucle, tras lo que guardan su PID en el archivo de los 
* procesos, bajan el semaforo que lo controla y mandan una senal al
* proceso que sube otra vez el semaforo al recibirla.
*
* @return int que indica si el programa se ha ejecutado con exito o no.
*/
int main(){
	int i;
	int pids[NCAJAS];
	int temp;
	int ret;
	unsigned short *array;
	char nombre[500];
	FILE *f;
	FILE *fProcesoEspera;

	/*Inicializamos la semilla para generar los numeros aleatorios*/
	srand(time(NULL));

	/*Inicializamos el array de semaforos a 1*/
	array = (unsigned short *)malloc(sizeof(unsigned short)*N_SEMAFOROS);
	if(array == NULL){
		printf("Error al reservar memoria\n");
	}

	for(i = 0; i < N_SEMAFOROS; i++){
		array[i] = 1;
	}

	/*Creamos e inicializamos los semaforos a 1*/
	ret = Crear_Semaforo(SEMKEY, N_SEMAFOROS, &semid);
	if(ret == ERROR){
		perror("Error al crear los semaforos");
		exit(EXIT_FAILURE);
	}

	ret = Inicializar_Semaforo(semid, array);
	if(ret == ERROR){
		perror("Error al inicializar los semaforos");
		exit(EXIT_FAILURE);
	}

	/*Inicializamos la funcion de manejo*/
	if(signal(SIGUSR1, retirarParte)== SIG_ERR){
		perror("Signal 1");
		exit(EXIT_FAILURE);
	}

	if(signal(SIGUSR2, retirarTotal)== SIG_ERR){
		perror("Signal 2");
		exit(EXIT_FAILURE);
	}
	
	/*Creamos los archivos para cada uno de los hijos, y a dichos hijos*/
	for(i = 0; i < NCAJAS; i++){
		/*Como el hijo sale del bucle y duplica toda la memoria
		del padre, conserva el valor nombre de su archivo.*/
		sprintf(nombre, "clientesCaja%d.txt", i);
		if(rellenar_fichero(nombre) == ERROR){
			exit(EXIT_FAILURE);
		}

		pids[i] = fork();
		if(pids[i] < 0){
			perror("Error al crear el proceso");
			exit(EXIT_FAILURE);
		}if(pids[i] == 0){
			break;
		}
	}

	/*El código que ejecuta cada uno de los hijos*/
	if(i < NCAJAS && pids[i] == 0){
		/*Abrimos el fichero con todas las operaciones
		de la caja, que no hace falta cerrar pues solo
		se utiliza desde dicha caja.*/
		f = fopen(nombre, "r");
		if(f == NULL){
			exit(EXIT_FAILURE);
		}
		
		while(fscanf(f, "%d\n", &temp) == 1){
			temp = mod_caja(i, temp, 0);
			printf("Dinero actual caja %d: %d\n", i, temp);
			/*Dormimos y atendemos al cliente*/
			sleep(aleat_num(1, 5));
			if(temp > MAX_DINERO){
				ret = Down_Semaforo(semid, N_SEMAFOROS - 1, SEM_UNDO);
				if(ret == ERROR){
					printf("Error al bajar semaforo 2, %d\n", i);
				}

				//printf("Soy la caja %d y acabo de avisar al padre.\n", i);
				fProcesoEspera = fopen("procesoEspera.txt", "w");
				fprintf(fProcesoEspera, "%d", i);
				fclose(fProcesoEspera);
				kill(getppid(), SIGUSR1);
			}
		}
		fclose(f);

		/*Una vez hemos acabado, avisamos al padre*/
		ret = Down_Semaforo(semid, N_SEMAFOROS - 1, SEM_UNDO);
		if(ret == ERROR){
			printf("Error al bajar semaforo 2, %d\n", i);
		}
		printf("Soy la caja %d y ya he acabado.\n", i);
		fProcesoEspera = fopen("procesoEspera.txt", "w");
		fprintf(fProcesoEspera, "%d", i);
		fclose(fProcesoEspera);
		kill(getppid(), SIGUSR2);
	}else{
		/*El padre simplemente tiene que esperar a los hijos*/
		while(wait(NULL) > 0);
		printf("\nSoy el padre, y en total he recaudado %d€.\n", cuentaGlobal);
		ret = Borrar_Semaforo(semid);
		if(ret == ERROR){
			exit(EXIT_FAILURE);
		}
	}

	free(array);
	exit(EXIT_SUCCESS);
}
