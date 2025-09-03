
NPROC=3

main: main.c
	mpicc $^ -o $@

run: main
	mpiexec -n ${NPROC} main

clean:
	- rm main

.PHONY:clean
