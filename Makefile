default:	build
clean:
	rm -rf Makefile build
rmobj:
	rm build/atmc
	find build/src -type f -name '*.o' -delete
.PHONY:build
build:
	$(MAKE) -f build/Makefile
install:
	$(MAKE) -f build/Makefile install
test:	build
	build/atmc test
