CC = clang

CFLAGS = -pipe -O0 -Wall -Wextra -Wpointer-arith -Wconditional-uninitialized -Wno-unused-parameter -Werror -g 

LINK = $(CC)

CORE_DEPS =

CORE_INCS = -I ../../../src/core \
	-I ../../../src/os \
	-I ../../../src/os/unix \
	-I ../../../src/os/unix/linux \
	-I ../../../build


misc:	misc.o
	$(LINK) -o misc.bin misc.o -ldl -lpthread -Wl,-E
misc.o:	$(CORE_DEPS)
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o misc.o misc.c


cache:	cache.o
	$(LINK) -o cache.bin cache.o -ldl -lpthread -Wl,-E
cache.o:	$(CORE_DEPS)
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o cache.o cache.c


.PHONY:clean
clean:
	rm -rf *.o *.bin
