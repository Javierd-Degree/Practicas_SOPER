#include <unistd.h>
#include "Ejercicio8.h"

#define NCAJAS 10
#define ERROR -1
#define OK 0

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
    if(sup < inf) return ERR;
    if(sup == inf) return sup;
    /*Cuando nuestro intervalo es mayor que RAND_MAX, groups será 0, y no funcionará*/
    if(sup-inf+1 > RAND_MAX) return ERR;
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
		return ERROR;
	}

	f = fopen('w', nombre);
	if(f == NULL){
		return ERROR;
	}

	for(i = 0; i < 50; i++){
		fprintf(f, "%d\n", aleat_num(0, 300));
	}

	fclose(f);
	return OK;
}

int sumar_caja(int caja, int valor){
	char nombre[NCAJAS/10 + 4];
	FILE *f = NULL;
	int temp = 0;

	sprintf(nombre, "%d", i);
	/*Cogemos el valor actual, o creamos el archivo si no existe*/
	f = fopen('r', nombre);
	if(f == NULL){
		f = fopen('w', nombre);
	}else{
		fscanf(f, "%d", temp);
		fclose(f);
		f = fopen('w', nombre);
	}

	fprintf(f, "%d", valor + temp);
	fclose(f);
	return valor + temp;
}

/*Los nombres de fichero de cada caja son i.txt, con i 
el numero de caja*/
int main(){
	int i;
	int pids[NCAJAS];
	int temp;
	int valor;
	char *nombre[500];
	FILE *f;
	FILE *fCaja;

	for(i = 0; i < NCAJAS; i++){
		sprintf(nombre, "clientesCaja%d.txt", i+1);
		if(rellenar_fichero(nombre) == ERROR){
			exit(EXIT_FAILURE);
		}

		pids[i] = fork();
		if(pids[i] < 0){
			perror("Error al crear el proceso");
			exit(EXIT_FAILURE);
		}
	}

	/*El código que ejecuta cada uno de los hijos*/
	if(pids[i] == 0){
		/*Abrimos el fichero con todas las operaciones
		de la caja, que no hace falta cerrar pues solo
		se utiliza desde dicha caja.*/
		valorCaja = 0;
		f = fopen("r", nombre);
		if(f == NULL){
			exit(EXIT_FAILURE);
		}

		while(fscanf(nombre, "%d\n", temp) == 1){
			/*TODO BAJAR SEMAFORO*/
			temp = sumar_caja(i, temp);
			/*TODO SUBIR SEMAFORO*/
			/*Dormimos y atendemos al cliente*/
			sleep(aleat_num(1, 5));
			/*TODO MANDAMOS ALERTA AL PADRE*/
		}
		fclose(f);
	}

	while(wait(NULL) > 0);
	exit(EXIT_SUCCESS);
}
