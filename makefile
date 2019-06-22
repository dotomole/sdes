CC = gcc
CFLAGS = -Wall -pedantic -ansi
OBJ = sdes.o
EXEC = sdes

$(EXEC) : $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

sdes.o : sdes.c sdes.h
	$(CC) -c sdes.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)