CC = gcc
CFLAGS = -Wall -Wextra -g -fPIC 

SRC_DIR = src
BUILD_DIR = build
INC_DIR = include

.PHONY: all liblwp dirs clean gdb

all: dirs liblwp lwp_test

# Build library
liblwp: liblwp.so liblwp.a

liblwp.so: $(BUILD_DIR)/lwp.o $(BUILD_DIR)/schedulers.o
	$(CC) $(CFLAGS) -shared -o $@ $^ 

liblwp.a: $(BUILD_DIR)/lwp.o $(BUILD_DIR)/schedulers.o
	ar rcs $@ $^ 

$(BUILD_DIR)/lwp.o: $(SRC_DIR)/lwp.c $(INC_DIR)/lwp.h 
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@ 

$(BUILD_DIR)/schedulers.o: $(SRC_DIR)/schedulers.c $(INC_DIR)/schedulers.h
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

dirs: 
	@mkdir -p $(BUILD_DIR)

# TEST PROGRAM
lwp_test: $(BUILD_DIR)/lwp_test.o liblwp
	$(CC) $(CFLAGS) -L. -o $@ $< -llwp

$(BUILD_DIR)/lwp_test.o: $(SRC_DIR)/lwp_test.c $(INC_DIR)/lwp.h
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@ 

gdb: lwp_test
	gdb ./$<
	lay next

clean:
	rm -rf $(BUILD_DIR) *.a *.so lwp_test
