.PHONY: release
release:
	cd src/compiler; make release

.PHONY: debug
debug:
	cd src/compiler; make debug

.PHONY: clean
clean:
	cd src/compiler; make clean
