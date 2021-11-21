rebuild:
	@echo "Rebuilding project..."
	@rm -f 42sh
	@ninja -C builddir
	@cp builddir/42sh .
	@echo "Done."

build:
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

check: build
	./tests/parser.sh

clean:
	@echo "Cleaning up..."
	@rm -rf builddir 42sh doc
	@echo "Done."
