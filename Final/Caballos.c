#include "Caballos.h"


/**
* Funcion a la que le pasas 2 numeros y devuelve un numero aleatorio entre ambos.
*
* @param inf Int con el numero mas bajo que se quiere.
* @param sup Int con el numero mas alto que se quiere.
* @return int con un número aleatorio entre inf y sup. 
*/
int dado(int inf, int sup){
    if(sup < inf) return -1;
    if(sup == inf) return sup;
    if(sup-inf+1 > RAND_MAX) return -1;
    int random;
    int range = sup-inf+1;
    int groups = RAND_MAX/range;
    int new_lim = groups * range;

    do{
    	random = rand();
    }while(random >= new_lim);

    return (random/groups) + inf;
}

int caballoAvanza(int modo){
	if(modo == MEDIO){
		return dado(1, 6);
	}else if(modo == PRIMERO){
		return dado(1, 7);
	}else if(modo == ULTIMO){
		return dado(2, 12);
	}

	return -1;
}

/*Cuenta el numero de caballos que han acabado la carrera, con 
lo que nos permite saber en que posicion ha qudado cada uno.
Para esto, recibe un array con los pid de los procesos, y los
caballos que ya hayan terminado, tendran pid -1*/
int carreraAcabada(int *pids, int numCaballos){
	int i;
	int n;
	for(i = 0, n = 0; i < numCaballos; i++){
		if(pids[i] == -1){
			n++;
		}
	}
	return n;
}

/*Dada la lista de caballos, nos permite saber si un caballo
esta en el medio, al final o al principio.*/
int posicionCaballo(int num, int *lista, int numCaballos){
	int i;
	int esMax = 1;
	int esMin =  1;

	/*En la primera tirada tiramos un dado normal.*/
	if(lista[num] == 0){
		return MEDIO;
	}

	for(i = 0; i < numCaballos; i++){
		if(i == num) continue;
		if(lista[num] <= lista[i]){
			esMax = 0;
		}else if(lista[num] > lista[i]){
			esMin = 0;
		}
	}

	if(esMax == 1){
		return PRIMERO;
	}else if(esMin == 1){
		return ULTIMO;
	}

	return MEDIO;
}

void corre(){
	return;
}

void caballo(int numero, int pipe[2], int lonCarrera){
	mensajeCaballo mensaje;
	key_t key;
	int mensaje_id;
	int posicion;
	int recorrido;
	int res;
	int temp;
	char sPosicion[20];


	/*Inicializamos la semilla para generar los numeros aleatorios*/
	srand(time(0)*numero);

	/*El caballo solo puede leer del pipe*/
	close(pipe[1]);

	if(signal(SENAL_CABALLO, corre) == SIG_ERR){
		printf("Error al inicializar el manejador.\n");
	}

	key = ftok(FILE_CABALLO_KEY, CABALLO_KEY);
	if(key ==(key_t) -1){
		printf("Error al obtener la clave de la cola de mensajes.");
	}

	mensaje_id = msgget(key, IPC_CREAT | 0660);
	if(mensaje_id == -1){
		printf("Error al crear la cola de mensajes en caballo.");
	}

	recorrido = 0;
	while(1){
		/*El padre le avisa cuando ha escrito en el pipe*/
		//pause();
		//printf("Soy el caballo %d, quiero leer.\n", numero);
		read(pipe[0], sPosicion, sizeof(sPosicion));
		posicion = atoi(sPosicion);

		temp = caballoAvanza(posicion);
		recorrido += temp;

		sprintf(mensaje.text, "%d", temp);
 		mensaje.type = numero;

 		//printf("Soy el caballo %d llevo %d\n", numero, recorrido);
 		printf("Soy el caballo %d, voy en posicion %d, y llevo %d\n", numero, posicion, recorrido-temp);

		res = msgsnd(mensaje_id, (struct msgbuf*)&mensaje, sizeof(mensaje) - sizeof(long), IPC_NOWAIT);
		if(res == -1){
			printf("Error al enviar el mensaje.");
		}

		if(recorrido >= lonCarrera){
			printf("Soy el caballo %d y he acabado la carrera.\n", numero);
			break;
		}
	}

	return;
}