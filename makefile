
functions= main.o functionsFiles.o functionsData.o functionsSort.o functionsAllToAllv.o functionsCreateType.o temp.o timestamp.o verify.o functionsSeqTree.o

main: $(functions)
	mpicc -o main $(functions) -lm


%.o: %.c headerFuncs.h
	mpicc -c -o $@ $< -I.

clean:
	rm *~ main *.o
