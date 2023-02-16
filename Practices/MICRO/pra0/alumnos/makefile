#MAKEFILE OF P0 EX3

#EXECUTABLE FILE WE WANT TO OBTAIN
all: alumno.exe

#TO DO THE LINK OF PROGRAMS AND GENERATE THE EXECUTABLE
alumno.exe: alumno.obj
	tlink /v alumno

#TO DO THE ASSEMBLY AND GENERATE THE OBJECT FILE
alumno.obj: alumno.asm
	tasm /zi alumno.asm ,, alumno.lst