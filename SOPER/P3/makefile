CC = gcc
CFLAGS = -g -Wall -pedantic 
EXE1 = monitor
EXE2 = miner
LIBS = -lrt

all: $(EXE1) $(EXE2)

######## EJECUTABLE ############################
$(EXE1): monitor.o pow.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(EXE2): miner.o pow.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

######## ARCHIVOS .O ###########################

monitor.o: monitor.c monitor.h
	$(CC) $(CFLAGS) -c $< 

miner.o: miner.c miner.h pow.h
	$(CC) $(CFLAGS) -c $< 

pow.o: pow.c pow.h
	$(CC) $(CFLAGS) -c $<

######## LIMPIEZA DE ARCHIVOS #################
clean:
	rm -rf *.o $(EXE1) $(EXE2) 