
default:	build

clean:
	rm -rf Makefile build
.PHONY:build

build:
	$(MAKE) -f build/Makefile

install:
	$(MAKE) -f build/Makefile install

modules:
	$(MAKE) -f build/Makefile modules

upgrade:
	/usr/local/atmcache/sbin/atmcache -t

	kill -USR2 `cat /usr/local/atmcache/logs/atmcache.pid`
	sleep 1
	test -f /usr/local/atmcache/logs/atmcache.pid.oldbin

	kill -QUIT `cat /usr/local/atmcache/logs/atmcache.pid.oldbin`
