CC = clang

CFLAGS = -pipe -O0 -Wall -Wextra -Wpointer-arith -Wconditional-uninitialized -Wno-unused-parameter -Werror -g 

LINK = $(CC)

CORE_DEPS =

CORE_INCS = -I ../../../src/core \
	-I ../../../src/os \
	-I ../../../src/os/unix \
	-I ../../../src/os/unix/linux \
	-I ../../../build

build:	binary 

binary:	misc.bin

misc.bin:	test.o

	$(LINK) -o misc.bin \
		test.o \
		-ldl -lpthread -Wl,-E


test.o:	$(CORE_DEPS)
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o test.o test.c

.PHONY:clean
clean:
	rm -rf *.o misc.bin
