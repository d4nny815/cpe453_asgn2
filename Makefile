CC = gcc
# CFLAGS = -Wall -Wextra -Werror -g -fPIC 
CFLAGS = -Wall -Wextra -g -fPIC 

SRC_DIR = src
BUILD_DIR = build
INC_DIR = include

.PHONY: all liblwp dirs clean gdb

all: dirs liblwp numbers 
#all: dirs liblwp lwp_test

# Build library
liblwp: liblwp.so liblwp.a

liblwp.so: $(BUILD_DIR)/lwp.o $(BUILD_DIR)/schedulers.o $(BUILD_DIR)/magic64.o
	$(CC) $(CFLAGS) -shared -o $@ $^ 

liblwp.a: $(BUILD_DIR)/lwp.o $(BUILD_DIR)/schedulers.o $(BUILD_DIR)/magic64.o
	ar rcs $@ $^ 

$(BUILD_DIR)/lwp.o: lwp.c lwp.h 
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@ 

$(BUILD_DIR)/schedulers.o: schedulers.c schedulers.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/magic64.o: magic64.S 
	$(CC) $(CFLAGS) -c $< -o $@

dirs: 
	@mkdir -p $(BUILD_DIR)

# Test Program 
lwp_test: $(BUILD_DIR)/lwp_test.o liblwp
	$(CC) $(CFLAGS) -L. -o $@ $< -llwp

$(BUILD_DIR)/lwp_test.o: lwp_test.c lwp.h
	$(CC) $(CFLAGS) -c $< -o $@ 

# Demo Program
numbers: $(BUILD_DIR)/numbersmain.o liblwp
	$(CC) $(CFLAGS) -L. -o $@ $< -llwp

$(BUILD_DIR)/numbersmain.o: demos/numbersmain.c lwp.h
	$(CC) $(CFLAGS) -c $< -o $@ 



clean:
	rm -rf $(BUILD_DIR) *.a *.so lwp_test numbers
