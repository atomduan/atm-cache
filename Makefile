default:	build
clean:
	rm -rf Makefile build
.PHONY:build
build:
	$(MAKE) -f build/Makefile
install:
	$(MAKE) -f build/Makefile install
