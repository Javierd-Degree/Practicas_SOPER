#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>

void main(int argc, char* argv){
	int pid, res;
	pid = fork();
	if(pid < 0){
			exit(EXIT_FAILURE);
			printf("Error al crear el proceso.\n");
		}else if(pid[i] == 0){
			int f = open(argv[1], "w");
			char* buffer;
			int i;
			struct stat buf;
			res = fstat(f, &buf);
			if(res == -1){
				perror("Error en fstat.");
			}

			for(i=0; i<buf.st_size, i+16000)
			buffer = (char*)mmap(0, 16000, PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
			if(buffer == MAP_FAILED){
				perror("Error en mmap.");
			}
		
			res = munmap(0, buf.st_size);
			if(res == -1){
				perror("Error en munmap.");
			}
			
			close(f);

		}else{
			pause();
		}
	pid = fork();
	if(pid < 0){
		exit(EXIT_FAILURE);
		printf("Error al crear el proceso.\n");
	}else if(pid[i] == 0){
		sleep(aleat_num(1, 5)); 
		printf("Introduce el nombre del nuevo usuario: ");
		scanf("%s", buffer);

		/* we declared to zone to share */
	   	inf = shmat (id_zone, (char*)0, 0);
		if(inf == NULL) {
	      	fprintf (stderr, "Error reserve shared memory \n");
			return -1; 
	   	}

	   	strcpy(inf->nombre, buffer);
	   	inf->id ++;

		kill(getppid(), SIGUSR1);

		free(array);

		exit(EXIT_SUCCESS);
	}else{
		pause();
	} 
}