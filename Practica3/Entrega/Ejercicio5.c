/**
* @brief Ejercicio 5 de la Practica 2.
*
* @file Ejercicio5.c
* @author Javier.delgadod@estudiante.uam.es 
* @author Javier.lopezcano@estudiante.uam.es
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define FILEKEY "/bin/ls"
#define KEY 33

typedef struct _message{
	long type;
	char text[16];
	int flag;
}message;

int id;
key_t key;


/**
* @brief El proceso padre crea una cola de mensajes y crea dos procesos hijo.
* El primero de estos realiza el proceso A que consiste en leer de un fichero
* y enviar a la cola de mensaje los datos del fichero en paquetes de 16 bytes.
* El segundo hijo se encarga del proceso B que consiste en acceder a la cola
* de mensajes y coger los mensajes del tipo 1 (los que ha puesto en la cola el
* proceso A) e ir modficando los caractere de este mensaje haciendo que cada 
* letra se traforme en la letra siguiente, tras lo cual envia la nueva cadena
* de caracteres a la cola con el tipo 2. Tras crear a los hijos el proceso padre
* pasa a hacerse cargo del proceso C que consiste en recibir de la cola de
* mensajes los mensajes de tipo 2 (los que ha enviado el proceso B) y los imprime
* en el fichero de salida. Para indicar a cada proceso cuando debe dejar de buscar
* elementos en la cola de mensajes se ha introducido en el mensaje un campo flag
* que es un int que es 0 cuando el proceso debe seguir recibiendo mensajes, pero
* cuando estos se acaban se envia un mensaje con el flag 1 de modo que el proceso
* que lo reciba sepa que se trata del ultimo mensaje.
* 
* @param argc int que indica el número de parametros de entrada.
* @param argv array de cadenas de caracteres que contiene todos los parametros de
* entrada, que en este caso son el nombre del ejecutable, y el nombre de los ficheros
* de entrada y de salida.
* @return int que determina si el programa se ha ejecutado o no con exito.
*/
int main(int argc, char** argv){
	int pid, res;

	if(argc != 3){
		printf("Error en el número de argumentos.\n");
		exit(EXIT_FAILURE);
	}

	/*Key de la cola de mensajes*/
	key = ftok (FILEKEY, KEY);
	if (key == (key_t)-1){
		perror("Error al obtener clave para cola mensajes\n");
		exit(EXIT_FAILURE);
	}

	/*Creamos la cola de mensajes*/
	id = msgget(key, IPC_CREAT | 0660);
	if(id == -1){
		perror("Error al crear la cola de mensajes\n");
	}

	/*Creamos el primer hijo*/
	pid = fork();
	if(pid < 0){
		exit(EXIT_FAILURE);
		printf("Error al crear el proceso.\n");
	}else if(pid == 0){
		/*Este primer hijo es el proceso A*/
		message mensaje;
		/*Abrimos el fichero*/
		FILE *f = fopen(argv[1], "r");
		
		/*Leemos el fichero*/
		while(fgets(mensaje.text, 16, f) != NULL){
			/*Ponemos el tipo de mensaje a 1 para que el proceso B pueda coger solo los del tipo 1*/
			mensaje.type = 1;
			/*Usamos una flag en el mensaje para indicar cual es el ultimo mensaje que el proceso B debe leer*/
			mensaje.flag = 0;
			/*Enviamos el mensaje a la cola*/
			res = msgsnd(id, (struct msgbuf*)&mensaje, sizeof(mensaje)-sizeof(long)-sizeof(int), IPC_NOWAIT);
			if(res == -1){
				perror("Error al enviar mensaje al proceso B.\n");
			}
		}
		mensaje.type = 1;
		/*Ponemos la flag a 1 y mandamos el ultimo mensaje que tendra que leer el proceso B*/
		mensaje.flag = 1;
		res = msgsnd(id, (struct msgbuf*)&mensaje, sizeof(mensaje)- sizeof(long)-sizeof(int), IPC_NOWAIT);
		if(res == -1){
			perror("Error al enviar mensaje al proceso B.\n");
		}
		
		/*Cerramos el ficherp*/
		fclose(f);

		exit(EXIT_SUCCESS);

	}else{
		/*Creamos el segundo hijo*/
		pid = fork();
		if(pid < 0){
			exit(EXIT_FAILURE);
			printf("Error al crear el proceso.\n");
		}else if(pid == 0){
			/*Este segundo hijo es el proceso B*/
			message mensaje;
			int i;
			while(1){
				/*Leemos mensajes del tipo 1 de la cola de mensajes*/
				res = msgrcv(id, (struct msgbuf*)&mensaje, sizeof(mensaje)-sizeof(long)-sizeof(int), 1, MSG_NOERROR);
				if(res == -1){
					perror("Error al recibir mensaje en proceso B.\n");
				}
				/*Si la bandera es 1 (es el ultimo mensaje que el proceso B tiene que leer) salimos del bucle*/
				if(mensaje.flag == 1){
					break;
				}
				
				/*Cambiamos cada letra del mensaje leido por la siguiente*/
				for(i=0; i<16; i++){
					if(mensaje.text[i] == 'z'){
						mensaje.text[i] = 'A';
					}else if(mensaje.text[i] == 'Z'){
						mensaje.text[i] = 'a';
					}else if((mensaje.text[i] >= 65 && mensaje.text[i] < 90)||(mensaje.text[i] >= 97 && mensaje.text[i] < 122)){
						mensaje.text[i]++;
					}
				}
				/*Ponemos el flag a 0 y el tipo al 2 para que el proceso C lea solo los mensajes de tipo 2*/
				mensaje.flag = 0;
				mensaje.type = 2;
				/*Enviamos el nuevo mensaje a la cola de mensajes*/
				res = msgsnd(id, (struct msgbuf*)&mensaje, sizeof(mensaje)-sizeof(long)-sizeof(int), IPC_NOWAIT);
				if(res == -1){
					perror("Error al enviar mensaje al proceso C.\n");
				}
			}
			/*Ponemos el flag a 1 y mandamos el ultimo mensaje que tendra que leer el proceso C*/
			mensaje.flag = 1;
			mensaje.type = 2;
			res = msgsnd(id, (struct msgbuf*)&mensaje, sizeof(mensaje)- sizeof(long)-sizeof(int), IPC_NOWAIT);
			if(res == -1){
				perror("Error al enviar mensaje al proceso C.\n");
			}
			exit(EXIT_SUCCESS);
		}else{
			/*El padre es el proceso C*/
			int i;
			/*Abrimos el fichero de salida*/
			FILE *w = fopen(argv[2], "w");
			message mensaje;

			while(1){
				/*Leemos los mensajes de tipo 2 de la cola*/
				res = msgrcv(id, (struct msgbuf*)&mensaje, sizeof(mensaje)-sizeof(long)-sizeof(int), 2, MSG_NOERROR);
				if(res == -1){
					perror("Error al recibir mensaje en proceso C.\n");
				}
				/*Si la flag es 1 (es el ultimo mensaje que debe leer el proceso C) salimos del bucle*/
				if(mensaje.flag == 1){
					break;
				}
				/*Imprimimos el mensaje en el fichero de salida*/
				fprintf(w, "%s", mensaje.text);
				
			}
			/*Cerramos el fichero*/
			fclose(w);
			/*Esperamos a que acaben los procesos hijo*/
			for(i=0; i<2; i++){
				wait(NULL);
			}
			/*Liberamos la cola de mensajes*/
			res = msgctl(id, IPC_RMID, (struct msqid_ds*)NULL);
			if(res == -1){
				perror("Error al eliminar cola de mensajes proceso C.\n");
			}
			exit(EXIT_SUCCESS);
		} 
	}
}