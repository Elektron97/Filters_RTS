#-----------------------------------------------------------------------
# Target file to be compiled by default
#-----------------------------------------------------------------------
MAIN = filters
#-----------------------------------------------------------------------
# CC is the compiler to be used
#-----------------------------------------------------------------------
CC = gcc
#-----------------------------------------------------------------------
# CFLAGS are the options passed to the compiler
#-----------------------------------------------------------------------
CFLAGS = -Wall -lpthread -lrt
#-----------------------------------------------------------------------
OBJ1 = taskLib
OBJ2 = timeLib
OBJ3 = filtersLib
OBJS = $(MAIN).o $(OBJ1).o $(OBJ2).o $(OBJ3).o
#-----------------------------------------------------------------------
# LIBS are the external libraried to be used
#-----------------------------------------------------------------------
LIBS = -lpthread -lrt -lm `allegro-config --libs`
#-----------------------------------------------------------------------
# Dependencies
#-----------------------------------------------------------------------
$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS) $(LIBS)

$(MAIN).o: $(MAIN).c
	$(CC) $(CFLAGS) -c $(MAIN).c

$(OBJ1).o: $(OBJ1).c
	$(CC) $(CFLAGS) -c $(OBJ1).c

$(OBJ2).o: $(OBJ2).c
	$(CC) $(CFLAGS) -c $(OBJ2).c

$(OBJ3).o: $(OBJ3).c
	$(CC) $(CFLAGS) -c $(OBJ3).c

.PHONY: clean

#clean:
#	rm *.o $(MAIN)

clean:
	rm -f	$(MAIN).o	$(MAIN) $(OBJ1).o	$(OBJ1) $(OBJ2).o	$(OBJ2) $(OBJ3).o	$(OBJ3)