###
### Authors: Matej Otčenáš(xotcen01), Mário Gažo(xgazom00)
### Date: 10.11.2020
### Filename: Makefile
###

# quiet makefile using prefix '@'
CC = @g++

CPPFLAGS = -std=c++11 -pedantic -Wall -Wextra -g -O2 -Wno-unused-parameter -lsimlib -lm 

PATHS = src/
PATHB = bin/

all: main

main: $(PATHS)main.cpp
	$(CC) $(CPPFLAGS) $^ -o $@ $(info    Compiling program...)

clean:
	rm -f main *.o
	