CC=gcc
CCFLAGS=-Wall -Wextra -Wpedantic
OPT=-O3

all: bin/counterd

bin/counterd: src/counterd.c
	$(CC) $(CCFLAGS) $(OPT) src/counterd.c -o bin/counterd
