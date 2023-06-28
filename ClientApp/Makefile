# Makefile for client application

CC = gcc
CFLAGS = -pthread -Wall -Werror -std=c99 -o
MAIN = main.c
AUX = client.c
NAME = ClientApp

all: 
	$(CC) $(CFLAGS) $(NAME) $(MAIN) $(AUX)

clean:
	rm $(NAME)
