/**
* @brief Ejercicio 8.
* Creamos distintos hijos para sustituirlos por los procesos indicados
* en la entrada del programa, usando tambien el exec indicado en la entrada.
*
* @file Ejercicio8.c
* @author Javier.delgadod@estudiante.uam.es Javier.lopezcano@estudiante.uam.es
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/**
* @brief Lanzamos un comando execl sobre un hijo creado.
* 
* lanzarExecl(char *comm) lanza el proceso comm usando execl.
* Usamos dos directorios, '/bin/' y '/usr/bien/'.
* @param comm proceso por el que se pretende lanzar usando execl.
*/

void lanzarExecl(char *comm){
	int pid;
	int status;
	char *commp, *commp2;

	commp = (char *)malloc(sizeof(char)*(strlen("/bin/")+strlen(comm)+1));
	if(commp == NULL){
		printf("Error al reservar memoria\n");
		return;
	}
	strcpy(commp, "/bin/");
	strcat(commp, comm);

	commp2 = (char *)malloc(sizeof(char)*(strlen("/usr/bin/")+strlen(comm)+1));
	if(commp2 == NULL){
		printf("Error al reservar memoria\n");
		return;
	}
	strcpy(commp2, "/usr/bin/");
	strcat(commp2, comm);

	if((pid = fork()) < 0){
		printf("Error al crear el primer hijo\n");
	}else if(pid == 0){
		execl(commp, comm, NULL);
		/* Solo se ejecuta si el anterior ha fallado, con lo que nos 
		aseguramos de que al menos, los tres del ejemplo, se ejecuten,
		pues están en los dos directorios*/
		execl(commp2, comm, NULL);
		perror("Fallo en execl\n");
	}else{
		while(wait(&status) != pid);
		if( status != 0){
			printf("Fallo en execl\n");
		}
	}

	free(commp);
	free(commp2);
}

/**
* @brief Lanzamos un comando execlp sobre un hijo creado.
* 
* lanzarExeclp(char *comm) lanza el proceso comm usando execlp.
* @param comm proceso por el que se pretende lanzar usando execlp.
*/
void lanzarExeclp(char *comm){
	int pid;
	int status;

	if((pid = fork()) < 0){
		printf("Error al crear el segundo hijo\n");
	}else if(pid == 0){
		execlp(comm, comm, NULL);
		perror("Fallo en execlp\n");
	}else{
		while(wait(&status) != pid);
		if( status != 0){
			printf("Fallo en execlp\n");
		}
	}
}


/**
* @brief Lanzamos un comando execv sobre un hijo creado.
* 
* lanzarExecp(char *comm) lanza el proceso comm usando execv.
* Usamos dos directorios, '/bin/' y '/usr/bien/'.
* @param comm proceso por el que se pretende lanzar usando execv.
*/
void lanzarExecv(char *comm){
	int pid;
	int status;
	char *commp, *commp2;
	char *cv[] = {comm, NULL};

	commp = (char *)malloc(sizeof(char)*(strlen("/bin/")+strlen(comm)+1));
	if(commp == NULL){
		printf("Error al reservar memoria\n");
		return;
	}
	strcpy(commp, "/bin/");
	strcat(commp, comm);

	commp2 = (char *)malloc(sizeof(char)*(strlen("/usr/bin/")+strlen(comm)+1));
	if(commp2 == NULL){
		printf("Error al reservar memoria\n");
		return;
	}
	strcpy(commp2, "/usr/bin/");
	strcat(commp2, comm);

	if((pid = fork()) < 0){
		printf("Error al crear el tercer hijo\n");
	}else if(pid == 0){
		execv(commp, cv);
		/* Solo nse ejecuta si el anterior ha fallado, con lo que nos 
		aseguramos de que al menos, los tres del ejemplo, se ejecuten,
		pues están en los dos directorios*/
		execv(commp2, cv);
		perror("Fallo en execv\n");
	}else{
		while(wait(&status) != pid);
		if( status != 0){
			printf("Fallo en execv\n");
		}
	}

	free(commp);
	free(commp2);
}

/**
* @brief Lanzamos un comando execvp sobre un hijo creado.
* 
* lanzarExecvp(char *comm) lanza el proceso comm usando execvp.
* @param comm proceso por el que se pretende lanzar usando execvp.
*/
void lanzarExecvp(char *comm){
	int pid;
	int status;

	char *cv[] = {comm, NULL};
	
	if((pid = fork()) < 0){
		printf("Error al crear el cuarto hijo\n");
	}else if(pid == 0){
		execvp(comm, cv);
		perror("Fallo en execvp\n");
	}else{
		while(wait(&status) != pid);
		if( status != 0){
			printf("Fallo en execvp\n");
		}
	}
}


/**
* @brief Llamamos a las funciones lanzarExecv, lanzarExecvp, 
* lanzarExecl o lanzarExeclp segun lo introducido en la entrada,
* y para cada uno de los comandos introducidos.
* 
* main (int argc, char* argv[]) Analiza la entrada y llama a las funciones
* con los parametros correspondientes
* @param argc numero de argumentos recibidos en la entrada.
* @param argv array de strings de argumentos recibidos en la entrada.
* 
* @return int que determina si el programa se ha ejecutado o no con exito.
*/
int main (int argc, char* argv[]) {
	int numProc;
	int i;

	// Descontamos el nombre del programa, y el tipo de exec a usar.
	numProc = argc - 2;
	printf("Numero de procesos: %d\n", numProc);

	if(strcmp(argv[argc-1], "-l") == 0){
		// execl
		for(i = 1; i <= numProc; i++){
			lanzarExecl(argv[i]);
		}

	}else if(strcmp(argv[argc-1], "-lp") == 0){
		// execlp
		for(i = 1; i <= numProc; i++){
			lanzarExeclp(argv[i]);
		}

	}else if(strcmp(argv[argc-1], "-v") == 0){
		// execv
		for(i = 1; i <= numProc; i++){
			lanzarExecv(argv[i]);
		}

	}else if(strcmp(argv[argc-1], "-vp") == 0){
		// execvp
		for(i = 1; i <= numProc; i++){
			lanzarExecvp(argv[i]);
		}

	}else{
		printf("Tipo de exec no valido\n");
	}


	exit(EXIT_SUCCESS);
}