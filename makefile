# Makefile for xyjson project
# Common commands integration for development workflow

.PHONY: build test build/fast test/fast install clean toc utable help

# Default target
help:
	@echo "Available targets:"
	@echo "  build      - Build the project using cmake and make"
	@echo "  test       - Build and test"
	@echo "  install    - Install the library"
	@echo "  clean      - Clean build directory"
	@echo "  toc        - Generate table of contents for header file"
	@echo "  utable     - Generate and update unit test table"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "make <target> -n  Show the command to execute only"

# All test source files
TEST_SOURCES := $(wildcard utest/*.cpp)

# Mark the last time some phony target executed
.touch:
	mkdir -p .touch

# Standard build
build/Makefile:
	mkdir -p build
	cd build && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
build: build/Makefile
	cd build && make

# Run tests (silent mode)
test: build
	./build/utxyjson --cout=silent

# Install the library
install: build
	cd build && make install

# Clean build directory
clean:
	rm -rf build

# Generate table of contents for header file
toc: .touch/toc.t
.touch/toc.t: .touch include/xyjson.h
	echo y | script/gen_toc.pl
	@touch $@

# Generate and update unit test table
utable: .touch/utable.t
.touch/utable.t: .touch $(TEST_SOURCES)
	perl script/utable.pl
	@touch $@

# Alias for help target
.DEFAULT_GOAL := help
