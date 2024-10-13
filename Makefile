CC = gcc
# CFLAGS = -Wall -Wextra -Werror -g -fPIC 
CFLAGS = -Wall -Wextra -g -fPIC 

BUILD_DIR = build

.PHONY: all liblwp dirs clean gdb

#all: dirs liblwp test_schedulers 
all: dirs liblwp numbers

# Build library
liblwp: liblwp.so liblwp.a

liblwp.so: $(BUILD_DIR)/lwp.o $(BUILD_DIR)/schedulers.o $(BUILD_DIR)/magic64.o
	$(CC) $(CFLAGS) -shared -o $@ $^ 

liblwp.a: $(BUILD_DIR)/lwp.o $(BUILD_DIR)/schedulers.o $(BUILD_DIR)/magic64.o
	ar rcs $@ $^ 

$(BUILD_DIR)/lwp.o: lwp.c lwp.h 
	$(CC) $(CFLAGS) -c $< -o $@ 

$(BUILD_DIR)/schedulers.o: schedulers.c schedulers.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/magic64.o: magic64.S 
	$(CC) $(CFLAGS) -c $< -o $@

dirs: 
	@mkdir -p $(BUILD_DIR)

# Demo Program
numbers: $(BUILD_DIR)/numbersmain.o liblwp
	$(CC) $(CFLAGS) -L. -o $@ $< -llwp

$(BUILD_DIR)/numbersmain.o: demos/numbersmain.c lwp.h
	$(CC) $(CFLAGS) -c $< -o $@ 

# His lib
test_schedulers: $(BUILD_DIR)/numbersmain.o $(BUILD_DIR)/schedulers.o
	$(CC) $(CFLAGS) -L./lib64 -o $@ $^ lib64/libPLN.so -lPLN

clean:
	rm -rf $(BUILD_DIR) *.a *.so core.* lwp_test numbers test_schedulers
