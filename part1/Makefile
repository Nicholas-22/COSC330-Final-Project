CC      = gcc
CFLAGS  = -Wall -Wextra -fPIC

.PHONY: all clean

all: libmylib.a libmylib.so shell

# Object file (position-independent so it can go into both library types)
mylib.o: mylib.c mylib.h
	$(CC) $(CFLAGS) -c mylib.c -o mylib.o

# Static library
libmylib.a: mylib.o
	ar rcs libmylib.a mylib.o

# Shared (dynamic) library
libmylib.so: mylib.o
	$(CC) -shared -o libmylib.so mylib.o

# Shell binary — linked against the shared library
shell: shell.c libmylib.so
	$(CC) $(CFLAGS) -o shell shell.c -L. -lmylib -Wl,-rpath,.

clean:
	rm -f mylib.o libmylib.a libmylib.so shell
