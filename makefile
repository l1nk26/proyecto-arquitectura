OBJETIVE=main3.cpp
CC=g++
CFLAGS=-Wall -Werror

all: $(OBJETIVE)
	$(CC) $(CFLAGS) -o ejecutable $(OBJETIVE)
	./ejecutable
clean:
	rm -f ejecutable