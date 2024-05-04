all : pilote simul

ihm.o : ihm.h ihm.c
	gcc -c ihm.c

simul.o : simul.c
	gcc -c simul.c

pilote.o : pilote.c
	gcc -c pilote.c

simul : simul.o ihm.o
	gcc -o simul simul.o ihm.o -lgraph -lm

pilote : pilote.o
	gcc -o pilote pilote.o

clean :
	rm -f *.o simul pilote

run :
	./pilote