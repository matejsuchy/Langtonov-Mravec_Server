OUTPUTS = server
CC = gcc

all: $(OUTPUTS)

clean:
	rm -f $(OUTPUTS)

.PHONY: all clean

server: main.c
	$(CC) -o server main.c