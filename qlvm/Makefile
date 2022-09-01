CC=gcc
CFLAGS=

QLVM=../qli
SRCS=$(wildcard src/*.c)
$(QLVM): $(SRCS) 
	$(CC) -o $(QLVM) $^ -I inc

clean:
	rm $(QLVM)
