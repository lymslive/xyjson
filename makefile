# Makefile for xyjson project
# Common commands integration for development workflow

.PHONY: build test build/fast test/fast install clean toc utable otable help release perf

# Default target
help:
	@echo "Available targets:"
	@echo "  build      - Build the project using cmake and make"
	@echo "  test       - Build and test"
	@echo "  install    - Install the library"
	@echo "  clean      - Clean build directory"
	@echo "  toc        - Generate table of contents for header file"
	@echo "  utable     - Generate and update unit test table"
	@echo "  otable     - Generate and update operator table"
	@echo "  release    - Build in release mode with performance tests"
	@echo "  perf       - Run performance tests (requires BUILD_PERF)"
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
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
build: build/Makefile
	cd build && make utxyjson

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
	./script/utable.sh
	@touch $@

# Generate and update operator table
otable: .touch/otable.t
.touch/otable.t: .touch docs/operator.csv
	./script/otable.sh
	@touch $@

# Build in Release mode with performance tests enabled
release: clean
	@echo "Building in Release mode with performance tests enabled..."
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DBUILD_PERF=ON ..
	cd build && make -j4

# Run performance tests (requires perf_test to be built)
perf: build
	@echo "Running performance tests..."
	cd build && make perf_test
	./build/perf_test

# Alias for help target
.DEFAULT_GOAL := help
