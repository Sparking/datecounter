CC     = gcc
CFLAGS = -O2 -Wall -std=c99
RM     = rm -f

ALL_O= counter.o main.o
ALL_T= datecnt

all: $(ALL_O)
	$(CC) -o $(ALL_T) $(ALL_O)
clean:
	$(RM) $(ALL_O)
counter.o: counter.c public.h
	$(CC) $(CFLAGS) -o $@ -c $<
main.o: main.c public.h version.h
	$(CC) $(CFLAGS) -o $@ -c $<
