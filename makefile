
functions= main.o functionsFiles.o functionsData.o functionsSort.o functionsMPI.o

main: $(functions)
	mpicc -o main $(functions)


%.o: %.c headerFuncs.h
	mpicc -c -o $@ $< -I.

#main.o: main.c
#	mpicc -c main.c
#	
#functionsFiles.o: functionsFiles.c
#	mpicc -c functionsFiles.c
#	
#functionsData.o: functionsData.c
#	mpicc -c functionsData.c
#	
#functionsSort.o: functionsSort.c
#	mpicc -c functionsSort.c



clean:
	rm *~ main *.o
