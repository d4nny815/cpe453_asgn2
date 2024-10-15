SHELL = /bin/sh

CC 	= gcc

LD 	= gcc

CFLAGS 	= -g -Wall -I$(LIBDIR) -I../shared -I.

LDFLAGS = -g -Wall -L$(LIBDIR) -llwp

DIRNAME = $(lastword $(subst /, ,$(CURDIR)))

ROOT    = $(DIRNAME)

PROG = $(ROOT)

SRCS 	= $(wildcard *.c) ../shared/sems.c

OBJS 	= $(SRCS:.c=.o)

REFFILE	= $(PROG).expected

LIBDIR 	= ../../lib

all:	$(REFFILE)

prog: $(PROG)

$(PROG): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

$(REFFILE): $(PROG) 
	@echo -n Regenerating $@...
	@LD_LIBRARY_PATH=$(LIBDIR):$$LD_LIBRARY_PATH;\
	export LD_LIBRARY_PATH;\
	$(PROG) > $(REFFILE)
	@echo done.

clean:
	-rm -f $(OBJS) *~

allclean: clean
	-rm -f $(PROG) $(REFFILE)
