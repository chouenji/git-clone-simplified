CC = gcc
CFLAGS = -Wall -Wextra -Werror

SRC = main.c
TARGET = main

INIT = init
PUSH = push 

all:
	@$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
	@./$(TARGET)

init:
	@$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
	@./$(TARGET) $(INIT)

push:
	@$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
	@./$(TARGET) $(PUSH)

