SOURCES=$(wildcard *.cpp)
HEADERS=$(SOURCES:.cpp=.h)
#FLAGS=-DDEBUG -g
FLAGS=-g -std=c++17

all: build

build: $(SOURCES) $(HEADERS) Makefile
	mpicxx $(SOURCES) $(FLAGS) -o app

clear: clean

clean:
	rm app

run: build Makefile
	mpirun -oversubscribe -np 8 ./app
