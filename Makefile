MAKE=make

.PHONY: all clean
all:
	$(MAKE) -C bootstrap
	$(MAKE) -C qlvm

clean:
	$(MAKE) -C bootstrap clean
	$(MAKE) -C qlvm clean
