CC = gcc
#CFLAGS = -Wall -Wextra -Werror -g -fPIC
CFLAGS = -Wall -Wextra -g -fPIC -std=c99

SRC_DIR = src
BUILD_DIR = build
INC_DIR = include

.PHONY: all liblwp dirs clean gdb

all: dirs liblwp lwp_test

# Build library
liblwp: dirs liblwp.so liblwp.a

liblwp.so: $(BUILD_DIR)/liblwp.o $(BUILD_DIR)/schedulers.o
	$(CC) $(CFLAGS) -shared -o $@ $< 

liblwp.a: $(BUILD_DIR)/liblwp.o $(BUILD_DIR)/schedulers.o
	ar rcs $@ $< 

$(BUILD_DIR)/liblwp.o: $(SRC_DIR)/lwp.c $(INC_DIR)/lwp.h 
	$(CC) $(CFLAGS) -c $< -o $@ 

$(BUILD_DIR)/schedulers.o: $(SRC_DIR)/

dirs: 
	mkdir -p $(BUILD_DIR)

# TEST PROGRAM
lwp_test: $(BUILD_DIR)/lwp_test.o liblwp
	$(CC) $(CFLAGS) -L. -o $@ $< -llwp

$(BUILD_DIR)/lwp_test.o: $(SRC_DIR)/lwp_test.c
	$(CC) $(CFLAGS) -c $< -o $@ 

gdb: lwp_test
	gdb ./lwp_test

clean:
	rm -rf $(BUILD_DIR) *.a *.so










