
($CFLAGS)=-g -Wall 
($MAINS)=Ejercicio1.c Ejercicio2.c Ejercicio3.c Ejercicio4.c Ejercicio5a.c Ejercicio5b.c Ejercicio6.c Ejercicio6dem.c test_wait.c Ejercicio7.c Ejercicio8.c Ejercicio9.c Ejercicio10.c Ejercicio11.c Ejercicio12a.c Ejercicio12b.c Ejercicio13a.c Ejercicio13b.c
($EJS)=Ejercicio1 Ejercicio2 Ejercicio3 Ejercicio4 Ejercicio5a Ejercicio5b Ejercicio6 Ejercicio6dem test_wait Ejercicio7 Ejercicio8 Ejercicio9 Ejercicio10 Ejercicio11 Ejercicio12a Ejercicio12b Ejercicio13a Ejercicio13b
######################################################################################################################################


make:
	($EJS): gcc ($MAINS) -lm -lpthread -o ($EJS)


clean:
	rm ($EJS)