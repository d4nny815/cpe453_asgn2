CC = gcc
# CFLAGS = -Wall -Wextra -Werror -g -fPIC 
CFLAGS = -Wall -Wextra -g -fPIC 

.PHONY: all liblwp clean gdb

all: liblwp 

# Build library
liblwp: liblwp.so liblwp.a

liblwp.so: lwp.o schedulers.o magic64.o
	$(CC) $(CFLAGS) -shared -o $@ $^ 

liblwp.a: lwp.o schedulers.o magic64.o
	ar rcs $@ $^ 

lwp.o: lwp.c lwp.h 
	$(CC) $(CFLAGS) -c $< -o $@ 

schedulers.o: schedulers.c schedulers.h
	$(CC) $(CFLAGS) -c $< -o $@

magic64.o: magic64.S 
	$(CC) $(CFLAGS) -c $< -o $@

# house keeping
clean:
	rm -rf *.a *.so *.o core.* lwp_test numbers test_schedulers 
