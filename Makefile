SOURCES=$(wildcard *.cpp)
HEADERS=$(SOURCES:.cpp=.hpp)
#FLAGS=-DDEBUG -g
FLAGS=-g -std=c++17 -O0

all: build

build: $(SOURCES) $(HEADERS) Makefile
	mpicxx $(SOURCES) $(FLAGS) -o app

clear: clean

clean:
	rm app

run: build Makefile
	mpirun -oversubscribe -np 16 ./app
