
functions = main.o functionsFiles.o functionsData.o functionsSort.o functionsMPI.o functionsAllToAllv.o functionsCreateType.o

main: $(functions)
	mpicc -o main $(functions)


%.o: %.c headerFuncs.h
	mpicc -c -o $@ $< -I.


clean:
	rm *~ main *.o
