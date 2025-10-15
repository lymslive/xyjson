# xyjson

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[English Version](README-en.md) | [中文文档](README.md)

C++ wrapper library for JSON operations, based on high-performance [yyjson](https://github.com/ibireme/yyjson), providing intuitive JSON processing experience through operator overloading.

## Project Name Meaning

- **xyjson** resembles **yyjson**, indicating dependency and tribute to the underlying high-performance library
- **x y** commonly used as mathematical symbols, xyjson aims to operate JSON data like mathematical variables through operator overloading

## Features

- 🚀 **High Performance** - Built on yyjson with zero-copy design
- ✨ **Intuitive Syntax** - Rich operator overloading, similar to native C++
- 🔒 **Type Safety** - Compile-time type checking, runtime-safe value extraction
- 📚 **Complete Functionality** - Supports read/write, iteration, file operations, etc.
- 🛠️ **Easy Integration** - **Header-only library**, CMake build support, `find_package` integration

## Dependencies

- **[yyjson](https://github.com/ibireme/yyjson)** - Core dependency, high-performance JSON parsing library
- **[couttast](https://github.com/lymslive/couttast)** - Optional dependency, only for testing and development

## Quick Start

### Direct Usage (Header-only Library)

**Copy header file directly (simplest way)**
```bash
# Only need to copy a single header file
cp include/xyjson.h your-project/include/
```

**Code Example:**
```cpp
#include "xyjson.h"

// Read JSON
std::string json = R"({"name": "Alice", "age": 30})";
xyjson::Document doc(json);

// Extract values
std::string name = doc / "name" | "unknown";
int age = doc / "age" | 0;
```

### CMake Integration

**Method 1: Using find_package**
```cmake
# In CMakeLists.txt
find_package(xyjson REQUIRED)
target_link_libraries(your-target PRIVATE xyjson::xyjson)
```

**Method 2: Standard build process**
```bash
# Clone repository
git clone <repository-url>
cd xyjson

# Build (dependencies will be auto-downloaded if not installed)
mkdir build && cd build
cmake .. && make

# Install
sudo make install
```

## Core Usage Examples

### Basic Operations

```cpp
// Create document
xyjson::Document doc(R"({"name": "Alice", "scores": [95, 87]})");

// Path access
std::string name = doc / "name" | "";

// Array access
int firstScore = doc / "scores" / 0 | 0;

// Type checking
bool isString = doc / "name" & "";
bool isNumber = doc / "age" & 0;
```

### Mutable Document Operations

```cpp
// Create mutable document
xyjson::MutableDocument mutDoc("{}");

// Assignment operations
mutDoc["name"] = "Bob";
mutDoc["scores"] = "[]";

// Array appending
mutDoc / "scores" << 95 << 87;

// File writing
mutDoc.writeFile("output.json");
```

### Iteration

```cpp
// Array iteration
for (auto iter = doc / "items" % 0; iter; ++iter) {
    std::cout << "Item " << iter->key << ": " << (iter->value | "") << std::endl;
}

// Object iteration  
for (auto iter = doc / "user" % ""; iter; ++iter) {
    std::cout << iter->key << " = " << (iter->value | "") << std::endl;
}
```

## Documentation Navigation

- 📖 [Usage Guide](docs/usage.md) - Detailed tutorials and best practices
- 🔧 [API Reference](docs/api.md) - Complete operator and class method documentation
- 🎨 [Design Philosophy](docs/design.md) - Library design concepts and philosophy
- 📋 [Development Requirements](task_todo.md) - Current project development requirements
- 📊 [Task Log](task_log.md) - AI collaboration task completion records

## Project Status

- ✅ **Stable and Usable** - Core functionality completed and tested
- 🔄 **Continuous Development** - Gradually improving features according to [requirements list](task_todo.md)
- 🧪 **Test Coverage** - Comprehensive unit tests ensure quality

## Build Options

### Compilation Requirements
- **C++ Standard**: C++17 or higher
- **Dependencies**: yyjson, couttast (testing)
- **Platforms**: Linux, macOS, Windows (MinGW)
- **Auto-dependencies**: FetchContent automatically downloads dependencies

### Continuous Integration

Project configured with GitHub Actions CI/CD pipeline:

- ✅ **Auto Build**: Automatic builds on main branch commits
- ✅ **Auto Testing**: Runs all unit tests
- ✅ **Dependency Management**: Automatically downloads and builds dependencies
- ✅ **Cross-platform**: Supports Ubuntu Linux environment

Check [Actions](https://github.com/lymslive/xyjson/actions) page for build status.

## Code Examples

The project doesn't have a separate `examples/` directory, but the `utest/` subdirectory contains comprehensive test cases that serve as detailed usage examples for learning and reference.

## Performance Optimization

Leveraging yyjson's high-performance characteristics, xyjson provides excellent performance while maintaining usability:

- **Zero-copy parsing** - Directly operates on raw JSON data
- **Efficient memory management** - RAII automatic resource release
- **String optimization** - Literal reference reduces copying

## Development Workflow

### Running Tests

```bash
cd build
./utxyjson --cout=silent  # Silent mode
./utxyjson                # Verbose output
```

### Code Structure

```
xyjson/
├── include/xyjson.h     # Main header file (header-only library)
├── utest/               # Unit tests (with rich examples)
├── docs/                # Detailed documentation
├── task_todo.md         # Development requirements management
├── task_log.md          # Task completion records
└── CMakeLists.txt       # Build configuration (supports find_package)
```

## Contributing

Welcome to submit Issues and Pull Requests!

## License

MIT License - See [LICENSE](LICENSE) file for details

## Related Projects

- [yyjson](https://github.com/ibireme/yyjson) - Underlying high-performance JSON library
- [couttast](https://github.com/lymslive/couttast) - Unit testing framework

---

*Simple and intuitive JSON operations, making C++ development more enjoyable!*