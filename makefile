.PHONY: release
release:
	cd src/compiler; make release
	cd src/unittests; make release

.PHONY: debug
debug:
	cd src/compiler; make debug
	cd src/unittests; make debug

.PHONY: test
test:
	bin/unittests

.PHONY: clean
clean:
	cd src/compiler; make clean
	cd src/unittests; make clean
