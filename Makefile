rebuild:
	@echo "Rebuilding project..."
	@rm -f 42sh
	@ninja -C builddir
	@cp builddir/42sh .
	@echo "Done."

build:
	@echo "Building project..."
	@meson setup builddir
	@ninja -C builddir
	@echo "Done."

check: build
	./tests/parser.sh

clean:
	@echo "Cleaning up..."
	@rm -rf builddir
	@echo "Done."
