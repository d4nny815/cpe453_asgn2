CC = gcc
# CFLAGS = -Wall -Wextra -Werror -g -fPIC 
CFLAGS = -Wall -Wextra -g -fPIC -I.

.PHONY: all liblwp clean gdb

# all: dirs liblwp numbers nico_test
all: liblwp numbers nico_test

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

# Demo Program
numbers: numbersmain.o liblwp
	$(CC) $(CFLAGS) -L. -o $@ $< -llwp

numbersmain.o: demos/numbersmain.c lwp.h
	$(CC) $(CFLAGS) -c $< -o $@ 

# His lib
test_schedulers: numbersmain.o schedulers.o
	$(CC) $(CFLAGS) -L./lib64 -o $@ $^ lib64/libPLN.so -lPLN

# target test case
nico_test: nico_test.o rr.o
	$(CC) $(CFLAGS) -L. -o $@ $^ -llwp

nico_test.o: nico_test.c
	$(CC) $(CFLAGS) -c $< -o $@ 

rr.o: rr.c
	$(CC) $(CFLAGS) -c $< -o $@

# house keeping
clean:
	rm -rf *.a *.so *.o core.* lwp_test numbers test_schedulers nico_test
