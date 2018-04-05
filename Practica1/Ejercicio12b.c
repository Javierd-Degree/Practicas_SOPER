#include <stdio.h> 	
#include <stdlib.h> 	
#include <string.h>
#include <math.h> 
#include <unistd.h> 	
#include <pthread.h>
#include <sys/time.h>
#include <time.h> 

typedef struct _estructura{
	char cad[100];
	int num;
}estructura;

int es_primo(int n){
	int i;
	for(i=2; i < sqrt(n); i++){
		if(n%i == 0){
			return 0;
		}
	}
	return 1;
}

void* calcular_primos(void *arg){
	estructura *est;
	int numPrimos;
	int i;
	int N;

	est = (estructura *)arg;
	N = est->num;

	for(i=1, numPrimos=0; numPrimos < N; i++){
		if(es_primo(i) == 1){
			numPrimos++;
		}
	}
	return NULL;
}	

int main(int argc , char *argv[]) { 	
	pthread_t h[100];
	estructura est;
	int i;
	double tiempo;
	struct timeval ti, tf;

	if(argc < 2){
		printf("No hay suficientes argumentos de entrada. Introduce el N.\n");
		exit(EXIT_FAILURE);
	}

	est.num = atoi(argv[1]);

	gettimeofday(&ti, NULL);
	for(i = 0; i < 100; i++){
		pthread_create(&h[i], NULL , calcular_primos , &est); 	
	}

	for(i = 0; i < 100; i++){
		pthread_join(h[i],NULL); 
	} 

	gettimeofday(&tf, NULL);
	tiempo = (tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000.0;
	printf("El programa termino correctamente tardando: %lf ms\n", tiempo); 
	exit(EXIT_SUCCESS);
}