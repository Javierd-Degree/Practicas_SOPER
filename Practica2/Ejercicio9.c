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

/*Delvuelve la cantidad total guardada en el fichero.*/
/*Modo puede ser 0 para sumar, 1 para restar, 2 para vaciar la caja
En el caso en el que vaciamos la caja, valor se ignora.*/
int mod_caja(int caja, int valor, int modo){
	char nombre[NCAJAS/10 + 4];
	FILE *f = NULL;
	int temp = 0;
	int result;

	sprintf(nombre, "%d", i);
	/*TODO BAJAR SEMAFORO*/
	/*Bajamos el semaforo para asegurarnos de que no leemos y escribimos
	a la vez desde el padre y el hijo.*/
	/*Cogemos el valor actual, o creamos el archivo si no existe*/
	f = fopen('r', nombre);
	if(f == NULL){
		f = fopen('w', nombre);
	}else{
		fscanf(f, "%d", &temp);
		fclose(f);
		f = fopen('w', nombre);
	}

	if(modo == 0){
		fprintf(f, "%d", temp + valor);
		result = temp + valor;
	}else if(modo == 1){
		fprintf(f, "%d", temp - valor);
		result = valor;
	}else{
		fprintf(f, "0");
		result = temp;
	}
	
	fclose(f);
	/*TODO SUBIR SEMAFORO*/
	return result;
}

/*Dado que antes de mandar la señal que llama a esta funcion, el
proceso hijo hace un down del semaforo, ningun otro proceso podra
lanzar dicha señal, y por tanto no es necesario usar una mascara 
se señales para bloquearlas.*/
void retirarParte(){
	FILE *f;
	int i;
	/*TODO, ¿Como seleccionamos el proceso?
	Deshabilitar y habilitar las dos señales*/
	f = fopen("r", "procesoEspera.txt");
	if(f == NULL){
		return;
	}
	
	if(fscanf(f, "%d", &i) != 1 || i < 0 || i >= NCAJAS){
		printf("Error al intentar saber que proceso ha mandado la señal.\n");
		return;
	}
	
	cuentaGlobal += mod_caja(i, 900, 1);
	/*TODO Subir semaforo archivo procesoEspera.txt*/
}

/*Dado que antes de mandar la señal que llama a esta funcion, el
proceso hijo hace un down del semaforo, ningun otro proceso podra
lanzar dicha señal, y por tanto no es necesario usar una mascara 
se señales para bloquearlas.*/
void retirarTotal(){
	cuentaGlobal += mod_caja(, 0, 2);
	/*TODO Subir semaforo archivo procesoEspera.txt*/
}


/*Los nombres de fichero de cada caja son i.txt, con i 
el numero de caja
Usamos SIGUSR1 cuando queremos que se retire dinero de nuestra caja
y SIGUSR2 cuando ya hemos acabado.
Usamos un archivo procesoEspera.txt para que el padre sepa que proceso 
ha mandado la señal. Lo protegemos con un semaforo para evitar que otro
proceso pueda sobreescribirlo etc.
*/
int cuentaGlobal;
int main(){
	int i;
	int pids[NCAJAS];
	int temp;
	int valor;
	char *nombre[500];
	FILE *f;
	FILE *fCaja;
	FILE *fProcesoEspera;

	cuentaGlobal = 0;
	/*Inicializamos la funcion de manejo*/
	if(signal(SIGUSR1, retirarParte)== SIG_ERR){
		perror("Signal 1");
		exit(EXIT_FAILURE);
	}

	if(signal(SIGUSR2, retirarTodo)== SIG_ERR){
		perror("signal");
		exit(EXIT_FAILURE);
	}


	for(i = 0; i < NCAJAS; i++){
		/*Como el hijo sale del bucle y duplica toda la memoria
		del padre, conserva el valor nombre de su archivo.*/
		sprintf(nombre, "clientesCaja%d.txt", i+1);
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
			temp = mod_caja(i, temp, 0);
			/*Dormimos y atendemos al cliente*/
			sleep(aleat_num(1, 5));
			if(temp > 1000){
				/*TODO Down semaforo procesoEspera.txt*/
				fProcesoEspera = fopen("w", "procesoEspera.txt");
				fprintf(fProcesoEspera, "%d", i);
				kill(getppid(), SIGUSR1);
			}
		}
		fclose(f);
	}

	/*TODO Down semaforo procesoEspera.txt*/
	fProcesoEspera = fopen("w", "procesoEspera.txt");
	fprintf(fProcesoEspera, "%d", i);
	kill(getppid(), SIGUSR2);

	while(wait(NULL) > 0);
	exit(EXIT_SUCCESS);
}
