LIBRARIES=-lcxl
CC=gcc -g -DDEBUG -Wall -Werror
CC_OBJ=$(CC) -fpic -o $@ -c $<
CC_LIB=$(CC) -shared $(LIBRARIES) -o $@
CC_TEST=$(CC) -o $@ $< -I src/ -L build/ -lcapirasure $(LIBRARIES)
OBJECTS=build/capi.o build/erasure.o

all: build/ build/libcapirasure.so

build/:
	mkdir build/

# Library objects
build/capi.o: src/capi.c src/capi.h build/
	$(CC_OBJ)

build/erasure.o: src/erasure.c src/erasure.h
	$(CC_OBJ)

build/libcapirasure.so: $(OBJECTS)
	$(CC_LIB) $(OBJECTS)

# Test builders
test/example: test/example.c build/libcapirasure.so
	$(CC_TEST)

test/file_erasure: test/file_erasure.c build/libcapirasure.so
	$(CC_TEST)

# Utility commands
install:
	echo "install command not yet implemented"

check: build/libcapirasure.so test/example test/file_erasure
	LD_LIBRARY_PATH=build/ ./test/example
	LD_LIBRARY_PATH=build/ ./test/file_erasure test/tinytest.bin

clean:
	rm -rf build/ test/example test/file_erasure
