SOURCES=$(wildcard *.c)
HEADERS=$(SOURCES:.c=.h)
#FLAGS=-DDEBUG -g
FLAGS=-g

all: main

main: $(SOURCES) $(HEADERS) Makefile
	mpicc $(SOURCES) $(FLAGS) -o main.out

clear: clean

clean:
	rm main a.out

run: main Makefile
	mpirun -oversubscribe -np 16 ./main
