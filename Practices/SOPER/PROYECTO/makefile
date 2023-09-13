CC = gcc
CFLAGS = -g -Wall -pedantic 
EXE1 = miner
EXE2 = monitor
LIBS = -lrt

all: $(EXE1) $(EXE2)

######## EJECUTABLE ############################
$(EXE1): minero.o pow.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(EXE2): comprobador.o pow.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

######## ARCHIVOS .O ###########################

minero.o: minero.c minero.h pow.h
	$(CC) $(CFLAGS) -c $< 

comprobador.o: comprobador.c comprobador.h minero.h pow.h
	$(CC) $(CFLAGS) -c $< 

pow.o: pow.c pow.h
	$(CC) $(CFLAGS) -c $<

######## LIMPIEZA DE ARCHIVOS #################
clean:
	rm -rf *.o $(EXE1) $(EXE2)  