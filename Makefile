CC = gcc
C_FLAGS = -c -Iinclude -Wall -fPIC
TEST_C_FLAGS = -c -Iinclude

INSTALL_PREFIX = /usr/local

LIB = gds
SO_LIB = lib$(LIB).so
SO_FLAGS = -shared

.PHONY: clean install uninstall

$(SO_LIB): build/gds_array.o build/gds_vector.o build/gds_misc.o
	$(CC) $(SO_FLAGS) $^ -o $@

build/gds_array.o: src/gds_array.c include/gds.h include/gds_array.h include/gds_misc.h | build
	$(CC) $(C_FLAGS) $< -o $@

build/gds_vector.o: src/gds_vector.c include/gds.h include/gds_vector.h include/gds_misc.h | build
	$(CC) $(C_FLAGS) $< -o $@

build/gds_misc.o: src/gds_misc.c include/gds_misc.h | build
	$(CC) $(C_FLAGS) $< -o $@

test: build/tests.o build build/gds_array.o build/gds_vector.o build/gds_misc.o
	$(CC) $^ -o test

build/tests.o: tests.c | build
	$(CC) $(TEST_C_FLAGS) $< -o build/tests.o

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

