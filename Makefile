LIBRARIES=-lcxl
CC_OBJ=gcc -g -fpic -Wall -o $@ -c $< 
CC_LIB=gcc -g -shared $(LIBRARIES) -o $@
CC_TEST=gcc -g -o $@ $< -Wall -I src/ -L build/ -lcapirasure $(LIBRARIES)
OBJECTS=build/capi.o build/erasure.o

all: build/ build/libcapirasure.so

build/:
	mkdir build/

build/capi.o: src/capi.c src/capi.h build/
	$(CC_OBJ)

build/erasure.o: src/erasure.c src/erasure.h
	$(CC_OBJ)

build/libcapirasure.so: $(OBJECTS)
	$(CC_LIB) $(OBJECTS)

check: build/libcapirasure.so test/example test/file_erasure
	LD_LIBRARY_PATH=build/ ./test/example
	LD_LIBRARY_PATH=build/ ./test/file_erasure test/tinytest.bin

test/example: test/example.c
	$(CC_TEST)

test/file_erasure: test/file_erasure.c
	$(CC_TEST)

clean:
	rm -rf build/
