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
/*De momento usamos solo dos semaforos, uno para los ficheros de 
las cajas, y otro para el fichero proceso espera.*/
#define N_SEMAFOROS (NCAJAS + 1)
#define MAX_DINERO 1000

int semid;
int cuentaGlobal = 0;

/*********************************************/
/* Funcion: aleat_num                        */
/* Fecha: 29-09-2017                         */
/* Autores:  Javier Delgado del Cerro,       */
/*           Javier López Cano               */
/*           Grupo 1201 Pareja 15            */
/*                                           */
/* Rutina que genera un numero aleatorio     */ 
/* entre dos numeros dados                   */
/*                                           */
/* Entrada:                                  */
/* int inf: limite inferior                  */
/* int sup: limite superior                  */
/* Salida:                                   */
/* int: numero aleatorio                     */
/*********************************************/
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

	for(i = 0; i < 10; i++){
		fprintf(f, "%d\n", aleat_num(0, 300));
	}

	fclose(f);
	return OK;
}

/*Delvuelve la cantidad total guardada en el fichero.*/
/*Modo puede ser 0 para sumar, 1 para restar, 2 para vaciar la caja
En el caso en el que vaciamos la caja, valor se ignora.*/
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
void retirarParte(){
	FILE *f;
	int i;
	int ret;
	/*Deshabilitar y habilitar las dos señales¿?*/
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


/*Los nombres de fichero de cada caja son i.txt, con i 
el numero de caja
Usamos SIGUSR1 cuando queremos que se retire dinero de nuestra caja
y SIGUSR2 cuando ya hemos acabado.
Usamos un archivo procesoEspera.txt para que el padre sepa que proceso 
ha mandado la señal. Lo protegemos con un semaforo para evitar que otro
proceso pueda sobreescribirlo etc.
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
	printf("%d, %d\n", sizeof(unsigned short), N_SEMAFOROS);
	array = (unsigned short *)malloc(sizeof(unsigned short)*N_SEMAFOROS);
	if(array == NULL){
		printf("Error al reservar memoria\n");
	}

	for(i = 0; i < N_SEMAFOROS; i++){
		array[i] = 1;
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
				/*TODO Down semaforo procesoEspera.txt. El padre hace el Up una vez lo ha leido.*/
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
		/*TODO Down semaforo procesoEspera.txt. El padre hace el Up una vez lo ha leido.*/
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
