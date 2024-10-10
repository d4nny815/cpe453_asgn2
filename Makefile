CC = gcc
#CFLAGS = -Wall -Wextra -Werror -g -fPIC
CFLAGS = -Wall -Wextra -g -fPIC

SRC_DIR = src
BUILD_DIR = build
INC_DIR = include

.PHONY: all liblwp dirs clean

all: dirs liblwp lwp_test

# Build library
liblwp: dirs liblwp.so liblwp.a

liblwp.so: $(BUILD_DIR)/liblwp.o
	$(CC) $(CFLAGS) -shared -o $@ $< 

liblwp.a: $(BUILD_DIR)/liblwp.o
	ar rcs $@ $< 

$(BUILD_DIR)/liblwp.o: $(SRC_DIR)/lwp.c $(INC_DIR)/lwp.h
	$(CC) $(CFLAGS) -c $< -o $@ 

dirs: 
	mkdir -p $(BUILD_DIR)

# TEST PROGRAM
lwp_test: $(BUILD_DIR)/lwp_test.o liblwp
	$(CC) $(CFLAGS) -L. -o $@ $< -llwp

$(BUILD_DIR)/lwp_test.o: $(SRC_DIR)/lwp_test.c
	$(CC) $(CFLAGS) -c $< -o $@ 



clean:
	rm -rf $(BUILD_DIR) *.a *.so










