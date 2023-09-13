CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi
# LDLIBS = -lodbc

# recompile if this header changes
HEADERS = utils.h

EXE1 = tester
EXE2 = menu
OBJ1 = $(EXE1).o utils.o checkUtils.o checkCreateTable.o checkReplaceExtensionByIdx.o \
checkCreateIndex.o checkPrint.o checkFindKey.o checkAddIndexEntry.o checkAddTableEntry.o
OBJ2 = $(EXE2).o utils.o

all : clean $(EXE1) $(EXE2)


%.o: %.c $(HEADERS)
	@echo Compiling $<...
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXE1): $(DEPS) $(OBJ1)
	$(CC) -o $(EXE1) $(OBJ1) $(LDLIBS)

$(EXE2): $(DEPS) $(OBJ2)
	$(CC) -o $(EXE2) $(OBJ2) $(LDLIBS)

run : $(DEPS)
	./$(EXE2)

clean :
	rm -f *.o core $(EXE1) $(EXE2)

