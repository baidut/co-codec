CC = gcc
CFLAGS = -Wall -std=c99 -pedantic
EXEC = aric
SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)

$(EXEC) : $(OBJS)
	$(CC) -o $(EXEC) $(OBJS)

%.o : %.c
	$(CC) $(CFLAGS) -c $(SRC)

clean : 
	rm $(OBJS) $(EXEC)

