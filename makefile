
CC = gcc

PROGS = cuddly-yaml

all: $(PROGS)

cuddly-yaml: main.c 
	$(CC) `pkg-config --cflags --libs glib-2.0` `pkg-config --cflags --libs libcyaml` -v main.c -o $(PROGS)


IGNORED = *.o $(PROGS)

.PHONY:
ignored:
	@echo $(IGNORED)

.PHONY:
clean:
	rm -f *.o $(PROGS)