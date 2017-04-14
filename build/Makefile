
CC =	cc
CFLAGS =  -pipe  -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Werror -g 
LINK =	$(CC)


ALL_INCS = -I src/core \
	-I build


CORE_DEPS = src/core/atmcache.h \
	src/core/atm_config.h \
	src/core/atm_core.h \
	src/core/atm_hash.h \
	src/core/atm_list.h \
	src/core/atm_string.h \
	build/atm_auto_config.h


CORE_INCS = -I src/core \
	-I build


build:	binary 

binary:	build/atmcache

build/atmcache:	build/src/core/atmcache.o \
	build/src/core/atm_core.o \
	build/src/core/atm_hash.o \
	build/src/core/atm_list.o \
	build/src/core/atm_siphash.o \
	build/src/core/atm_string.o

	$(LINK) -o build/atmcache \
	build/src/core/atmcache.o \
	build/src/core/atm_core.o \
	build/src/core/atm_hash.o \
	build/src/core/atm_list.o \
	build/src/core/atm_siphash.o \
	build/src/core/atm_string.o \
	-ldl -lpthread \
	-Wl,-E
	


build/src/core/atmcache.o:	$(CORE_DEPS) \
	src/core/atmcache.c
	$(CC) -c $(CFLAGS) $(CORE_INCS) \
		-o build/src/core/atmcache.o \
		src/core/atmcache.c


build/src/core/atm_core.o:	$(CORE_DEPS) \
	src/core/atm_core.c
	$(CC) -c $(CFLAGS) $(CORE_INCS) \
		-o build/src/core/atm_core.o \
		src/core/atm_core.c


build/src/core/atm_hash.o:	$(CORE_DEPS) \
	src/core/atm_hash.c
	$(CC) -c $(CFLAGS) $(CORE_INCS) \
		-o build/src/core/atm_hash.o \
		src/core/atm_hash.c


build/src/core/atm_list.o:	$(CORE_DEPS) \
	src/core/atm_list.c
	$(CC) -c $(CFLAGS) $(CORE_INCS) \
		-o build/src/core/atm_list.o \
		src/core/atm_list.c


build/src/core/atm_siphash.o:	$(CORE_DEPS) \
	src/core/atm_siphash.c
	$(CC) -c $(CFLAGS) $(CORE_INCS) \
		-o build/src/core/atm_siphash.o \
		src/core/atm_siphash.c


build/src/core/atm_string.o:	$(CORE_DEPS) \
	src/core/atm_string.c
	$(CC) -c $(CFLAGS) $(CORE_INCS) \
		-o build/src/core/atm_string.o \
		src/core/atm_string.c


install:	build 
	test -d '/usr/local/atmcache' || mkdir -p '/usr/local/atmcache'
	test -d '/usr/local/atmcache/sbin' || mkdir -p '/usr/local/atmcache/sbin'
	test ! -f '/usr/local/atmcache/sbin/atmcache' || mv '/usr/local/atmcache/sbin/atmcache' '/usr/local/atmcache/sbin/atmcache.old'
	cp build/atmcache '/usr/local/atmcache/sbin/atmcache'
	test -d '/usr/local/atmcache/config' || mkdir -p '/usr/local/atmcache/config'
	test -f '/usr/local/atmcache/config/atmcache.conf' || cp config/atmcache.conf '/usr/local/atmcache/config/atmcache.conf'
	cp conf/atmcache.conf '/usr/local/atmcache/config/atmcache.conf.default'
	test -d '/usr/local/atmcache/logs' || mkdir -p '/usr/local/atmcache/logs'
	test -d '/usr/local/atmcache/logs' || mkdir -p '/usr/local/atmcache/logs'
