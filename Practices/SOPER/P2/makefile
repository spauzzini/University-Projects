CC = gcc
CFLAGS = -g -Wall -pedantic
EXE = voting

all: $(EXE)

######## EJECUTABLE ############################
$(EXE): principal.o votante.o 
	$(CC) $(CFLAGS) -o $@ $^


######## ARCHIVOS .O ###########################
votante.o: votante.c votante.h
	$(CC) $(CFLAGS) -c $<

principal.o: principal.c principal.h votante.h 
	$(CC) $(CFLAGS) -c $<


######## LIMPIEZA DE ARCHIVOS #################
clean:
	rm -rf *.o $(EXE) *.txt



