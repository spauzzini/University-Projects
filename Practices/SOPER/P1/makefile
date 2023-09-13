CC = gcc
CFLAGS = -g -Wall -pedantic
EXE = mrush

all: $(EXE)

######## EJECUTABLE ############################
$(EXE): principal.o minero.o monitor.o pow.o
	$(CC) $(CFLAGS) -o $@ $^


######## ARCHIVOS .O ###########################
pow.o: pow.c pow.h
	$(CC) $(CFLAGS) -c $<

minero.o: minero.c minero.h pow.h
	$(CC) $(CFLAGS) -c $<

principal.o: principal.c principal.h minero.h
	$(CC) $(CFLAGS) -c $<

monitor.o: monitor.c monitor.h
	$(CC) $(CFLAGS) -c $<


######## LIMPIEZA DE ARCHIVOS #################
clean:
	rm -rf *.o $(EXE)


