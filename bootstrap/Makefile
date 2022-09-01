CC=gcc
CFLAGS=

QLC=../qlc
SRCS=$(wildcard src/*.c)
$(QLC): $(SRCS) 
	$(CC) -o $(QLC) $^ -I inc

clean:
	rm $(QLC)
