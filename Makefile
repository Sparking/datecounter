CC     = gcc
CPPFLAGS = -Wall -Werror -std=gnu99
CFLAGS = -O2
RM     = rm -f

ALL_C= counter.c main.c iniparser.c
ALL_O= $(patsubst %.c, %.o, $(ALL_C))
ALL_D= $(patsubst %.c, %.d, $(ALL_C))
ALL_T= datecnt

.PHONY: all
all: $(ALL_O)
	$(CC) -o $(ALL_T) $(ALL_O)
clean:
	$(RM) $(ALL_O)
-include $(ALL_D)
$(ALL_O): %.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -c -o $@ $<

.PHONY: clean
clean:
