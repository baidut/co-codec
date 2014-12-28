CC=gcc
CFLAGS =-I. -Wall -O2
#CFLAGS +=-DDebug -g
DEPS = io.h arithmetic.h
OBJ = main.o io.o arithmetic.o
#OBJ = io.o arithmetic.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

arithmetic: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *~
	rm -f core
