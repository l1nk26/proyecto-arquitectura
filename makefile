OBJETIVE=main3.cpp
CC=g++
CFLAGS=--Wall --Werror

all: $(OBJETIVE)
	$(CC) $(CFLAGS) -o main3 $(OBJETIVE)