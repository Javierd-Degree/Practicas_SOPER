#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_PROC 5
int main (void){
	int pid, counter;
	pid = fork();
	if (pid == 0){
		while(1){
			for (counter = 0; counter < NUM_PROC; counter++){
				printf("%d\n", counter);
				sleep(1);
			}
			sleep(3);
		}
	}
	while(wait(NULL)>0);
}