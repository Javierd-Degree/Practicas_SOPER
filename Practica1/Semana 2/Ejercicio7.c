#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_PROC 6

int main (void) {
	int pid, status;
	char *cv[] = {"ls", "-la", NULL};

	if((pid = fork()) < 0){
		printf("Error al crear el primer hijo\n");
	}else if(pid == 0){
		printf("\nUsamos execl\n");
		execl("/bin/ls", "ls", "-la", NULL);
		perror("Fallo en execl\n");
	}else{
		while(wait(&status) != pid);
		if( status != 0){
			printf("Fallo en execl\n");
		}
	}
	

	if((pid = fork()) < 0){
		printf("Error al crear el segundo hijo\n");
	}else if(pid == 0){
		printf("\nUsamos execlp\n");
		execlp("ls", "ls", "-la", NULL);
		perror("Fallo en execlp\n");
	}else{
		while(wait(&status) != pid);
		if( status != 0){
			printf("Fallo en execlp\n");
		}
	}


	if((pid = fork()) < 0){
		printf("Error al crear el tercer hijo\n");
	}else if(pid == 0){
		printf("\nUsamos execv\n");
		execv("/bin/ls", cv);
		perror("Fallo en execv\n");
	}else{
		while(wait(&status) != pid);
		if( status != 0){
			printf("Fallo en execv\n");
		}
	}

	if((pid = fork()) < 0){
		printf("Error al crear el cuarto hijo\n");
	}else if(pid == 0){
		printf("\nUsamos execvp\n");
		execvp("ls", cv);
		perror("Fallo en execvp\n");
	}else{
		while(wait(&status) != pid);
		if( status != 0){
			printf("Fallo en execvp\n");
		}
	}

	exit(EXIT_SUCCESS);
}

/* Al llamar a exec, se cambia la imagen del proceso del programa por la del proceso
al que estamos llamando, asi que lo hacemos con hijos para que el programa principal no 
acabe y se ejecuten todos*/