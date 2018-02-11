#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_PROC 6

int main (void) {
	int pid;
	int i;

	execlp("/bin/ls", "ls", "-la", NULL);
	execlp("ls", "ls", "-la", NULL);
	perror("fallo en exec");

	exit(EXIT_SUCCESS);
}	