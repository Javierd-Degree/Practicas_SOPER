
CC = gcc
CFLAGS = -g -Wall 
EJS = Carrera
######################################################################################################################################

all: $(EJS)

Carrera:
	$(CC) $(CFLAGS) Caballos.c Caballos.h GestorApuestas.c GestorApuestas.h main.c Monitor.c Monitor.h Semaforo.c Semaforo.h Utils.c Utils.h -lpthread -o Carrera

clean:
	rm -rf *o. $(EJS)
