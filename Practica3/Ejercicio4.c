#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/**
* Funcion a la que le pasas 2 numeros y devuelve un numero aleatorio entre ambos.
*
* @param inf Int con el numero mas bajo que se quiere.
* @param sup Int con el numero mas alto que se quiere.
* @return int con un número aleatorio entre inf y sup. 
*/
int aleat_num(int inf, int sup){
    if(sup < inf) return -1;
    if(sup == inf) return sup;
    /*Cuando nuestro intervalo es mayor que RAND_MAX, groups será 0, y no funcionará*/
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

void *writeFile(void* fichero){
	FILE *f = fopen((char*)fichero, "w");
	int i, MAX = aleat_num(1000, 2000);
	fprintf(f, "%d", aleat_num(100, 1000));
	for(i = 0; i < MAX - 1; i++){
		fprintf(f, " ");
		fprintf(f, "%d", aleat_num(100, 1000));
	}
	fclose(f);
	return NULL;
}

void *readFile(void* fichero){
	struct stat buf;
	int res;
	int f = open((char*)fichero, O_RDWR);
	char* buffer;
	int i;

	res = fstat(f, &buf);
	if(res == -1){
		perror("Error en fstat.");
	}
	buffer = (char*)mmap(0, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
	if(buffer == MAP_FAILED){
		perror("Error en mmap.");
	}
	for(i = 0; i < buf.st_size; i++){
		if(buffer[i] == ','){
			buffer[i] = ' ';
		}
		printf("%c", buffer[i]);
	}

	res = munmap(0, buf.st_size);
	if(res == -1){
		perror("Error en munmap.");
	}
	
	close(f);
	return NULL;
}


int main(int argc , char *argv[]) { 	
	pthread_t h[2];

	
	pthread_create(&h[0], NULL , (void*)writeFile , (void*)"fichero.txt"); 

	pthread_join(h[0],NULL);

	pthread_create(&h[1], NULL , (void*)readFile , (void*)"fichero.txt"); 	

	pthread_join(h[1],NULL); 

	exit(EXIT_SUCCESS);
}