CC=gcc
CFLAGS=

QLVM=qlvm
SRCS=$(wildcard src/*.c)
$(QLVM): $(SRCS) 
	$(CC) -o $(QLVM) $^ -I inc

clean:
	rm $(QLVM)
