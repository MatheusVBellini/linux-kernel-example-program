# Makefile for client application

CC = gcc
CFLAGS = -pthread -Wall -Werror -g -std=c99 -o
MAIN = main
AUX = client.c

all: 
	$(CC) $(CFLAGS) $(MAIN).o $(MAIN).c $(AUX)

clean:
	rm $(MAIN).o

run:
	./$(MAIN).o