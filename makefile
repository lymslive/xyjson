# Makefile for xyjson project
# Common commands integration for development workflow

.PHONY: build test build/fast test/fast install clean toc help

# Default target
help:
	@echo "Available targets:"
	@echo "  build      - Build the project using cmake and make"
	@echo "  build/fast - Make in the exist build directory"
	@echo "  test       - Build and test"
	@echo "  test/fast  - Test as if already build"
	@echo "  install    - Install the library"
	@echo "  clean      - Clean build directory"
	@echo "  toc        - Generate table of contents for header file"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "make <target> -n  Show the command to execute only"

# Standard build
build:
	mkdir -p build
	cd build && cmake .. && make

build/fast:
	cd build && make

# Run tests (silent mode)
test: build
	./build/utxyjson --cout=silent

test/fast:
	./build/utxyjson --cout=silent

# Install the library
install: build
	cd build && make install

# Clean build directory
clean:
	rm -rf build

# Generate table of contents for header file
toc:
	echo y | script/gen_toc.pl

# Alias for help target
.DEFAULT_GOAL := help
