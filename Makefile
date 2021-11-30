42sh: build

.PHONY: build

check: build
	@echo "Checking..."
	@python3 tests/moulinette.py --binary 42sh --tests tests/*.yml 
	@echo "Done."

rebuild:
	@echo "Rebuilding project..."
	@rm -f 42sh
	@ninja -C builddir
	@cp builddir/42sh .
	@echo "Done."

build: clean
	@echo "Building project..."
	@rm -f 42sh
	@meson setup builddir
	@ninja -C builddir
	@cp builddir/42sh .
	@echo "Done."

doc:
	@echo "Generating documentation..."
	@rm -rf doc
	@doxygen
	@echo "Done."

clean:
	@echo "Cleaning up..."
	@rm -rf builddir 42sh doc
	@echo "Done."
