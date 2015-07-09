# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Your platform. See PLATS for possible values.
PLAT= none

CC= gcc
CFLAGS= -O2 -Wall -std=c99

RM= rm -f

# == END OF USER SETTINGS -- NO NEED TO CHANGE ANYTHING BELOW THIS LINE =======

PLATS= linux mingw

ALL_O= counter.o main.o
ALL_T= datecnt

# Targets start here.
default: $(PLAT)

all:	$(ALL_T)

o:	$(ALL_O)

$(ALL_T): $(ALL_O)
	$(CC) -o $@ $(LDFLAGS) $(ALL_O)

clean:
	$(RM) $(ALL_T) $(ALL_O)

depend:
	@$(CC) $(CFLAGS) -MM *.c

echo:
	@echo "PLAT= $(PLAT)"
	@echo "CC= $(CC)"
	@echo "CFLAGS= $(CFLAGS)"
	@echo "RM= $(RM)"

# Convenience targets for popular platforms
ALL= all

none:
	@echo "Please do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATS)"

linux:
	$(MAKE) $(ALL)

mingw:
	$(MAKE) "ALL_T=datecnt.exe" datecnt.exe

# DO NOT DELETE

counter.o: counter.c public.h
main.o: main.c public.h version.h
