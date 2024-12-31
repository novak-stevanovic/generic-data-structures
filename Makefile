CC = gcc
C_OBJ_FLAGS = -c -Iinclude -Wall -fPIC

INSTALL_PREFIX = /usr/local

C_SRC = $(shell find src -name "*.c")
C_OBJ = $(patsubst src/%.c,build/%.o,$(C_SRC))

LIB = gends
SO_LIB = lib$(LIB).so
SO_FLAGS = -shared

.PHONY: clean install uninstall

$(SO_LIB): $(C_OBJ)
	$(CC) $(SO_FLAGS) $(C_OBJ) -o $(SO_LIB)

$(C_OBJ): build/%.o: src/%.c build
	mkdir -p $(dir $@)
	$(CC) $(C_OBJ_FLAGS) $< -o $@

test: $(C_OBJ) build
	gcc -c -Iinclude -Wall src/tests.c -o build/tests.o
	gcc $(C_OBJ) build/tests.o -o test

build:
	mkdir -p build/

install:
	sudo cp $(SO_LIB) $(INSTALL_PREFIX)/lib
	sudo mkdir -p $(INSTALL_PREFIX)/include/$(LIB)
	sudo cp -r include/* $(INSTALL_PREFIX)/include/$(LIB)

uninstall:
	sudo rm -f $(INSTALL_PREFIX)/lib/$(SO_LIB)
	sudo rm -rf $(INSTALL_PREFIX)/include/$(LIB)

clean:
	rm -rf build
	rm -f $(SO_LIB)
	rm -f compile_commands.json
	rm -f test

