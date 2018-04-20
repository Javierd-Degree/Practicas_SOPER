
CC = gcc
CFLAGS = -g -Wall 
EJS = Ejercicio2 Ejercicio2_solved Ejercicio3 Ejercicio4 Ejercicio5
######################################################################################################################################

all: $(EJS)

Ejercicio2:
	$(CC) $(CFLAGS) Ejercicio2.c -o Ejercicio2

Ejercicio2_solved:
	$(CC) $(CFLAGS) Ejercicio2_solved.c LibreriaSemaforos.h LibreriaSemaforos.c -o Ejercicio2_solved

Ejercicio3:
	$(CC) $(CFLAGS) Ejercicio3.c LibreriaSemaforos.h LibreriaSemaforos.c -o Ejercicio3

Ejercicio4:
	$(CC) $(CFLAGS) Ejercicio4.c -o Ejercicio4 -lpthread

Ejercicio5:
	$(CC) $(CFLAGS) Ejercicio5.c -o Ejercicio5


clean:
	rm -rf *o. $(EJS)