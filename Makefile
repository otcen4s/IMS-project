###
### Authors: Matej Otčenáš(xotcen01), Mário Gažo(xgazom00)
### Date: 10.11.2020
### Filename: Makefile
###

# quiet makefile using prefix '@'
CC = @g++

CPPFLAGS = -std=c++11 -pedantic -Wall -Wextra -g -O2 -Wno-unused-parameter

SRC = src/*.cpp
HDR = src/headers/*.h

all: main

main: $(SRC) $(HDR)
	$(CC) $(CPPFLAGS) $(SRC) -o $@ $(info    Compiling program...)

exp1: main
	./main scenario1
	python3 plot.py SIR

exp2: main
	./main scenario2
	python3 plot.py SEIRD

clean:
	rm -f main *.o
	