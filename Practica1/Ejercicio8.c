#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_PROC 6

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
		/* Solo nse ejecuta si el anterior ha fallado, con lo que nos 
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


/*Execv y exec l no funcionan con du, da error de directorio*/

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