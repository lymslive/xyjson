# xyjson

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[English Version](README-en.md) | [中文文档](README.md)

C++ wrapper library for JSON operations, based on high-performance [yyjson](https://github.com/ibireme/yyjson), providing intuitive JSON processing experience through operator overloading.

## Project Name Meaning

- Based on the yyjson library, the wrapped C++ classes act as proxies for the corresponding yyjson structures, and the namespace also continues to use `yyjson::`
- **x y** are commonly used as mathematical symbols, and the xyjson library aims to operate JSON data like mathematical variable symbols.

## Features

- 🚀 **High Performance** - Built on yyjson with zero-copy design
- ✨ **Intuitive Syntax** - Rich operator overloading, similar to native C++
- 🔒 **Type Safety** - Compile-time type checking, runtime-safe value extraction
- 📚 **Complete Functionality** - Supports read/write, iteration, file operations, etc.
- 🛠️ **Easy Integration** - **Header-only library**, CMake build support, `find_package` integration

## Dependencies

- **[yyjson](https://github.com/ibireme/yyjson)** - Core dependency, high-performance JSON parsing library
- **[couttast](https://github.com/lymslive/couttast)** - Optional dependency, only for testing and development
- **C++ Standard**: C++17 or higher
- **Platform**: Linux, macOS, Windows (MinGW)

## Quick Start

### Direct Usage Without Installation

After installing the underlying yyjson library, simply copy the single header file `include/xyjson.h` to an appropriate location in your project and use it out of the box.

**Code Example:**
<!-- example:readme_quick_start -->
```cpp
#include "xyjson.h"

// Read JSON
std::string json = R"({"name": "Alice", "age": 30})";
yyjson::Document doc(json);

// Extract values
std::string name = doc / "name" | ""; // Get "Alice"
int age = doc / "age" | 0;            // Get 30
```

### CMake Integration Installation

Supports standard CMake build process:
```bash
# Clone project
git clone https://github.com/lymslive/xyjson
cd xyjson

# Build (dependencies will be auto-downloaded if not installed)
mkdir build && cd build
cmake .. && make

# Install
sudo make install
```

Then use `find_package` integration in client projects:
<!-- example:NO_TEST -->
```cmake
# Use in CMakeLists.txt
find_package(xyjson REQUIRED)
target_link_libraries(your-target PRIVATE xyjson::xyjson)
```

## Core Usage Examples

### Basic Operations

<!-- example:readme_basic_ops -->
```cpp
// Create document object from JSON string, also can use doc << json_string to parse
yyjson::Document doc(R"({"name": "Alice", "scores": [95, 87]})");

// Path access
std::string name = doc / "name" | ""; // "Alice"

// Array access
int firstScore = doc / "scores" / 0 | 0; // 95

// Type checking
bool isString = doc / "name" & ""; // true
bool isNumber = doc / "scores" / 0 & 0;   // true
```

### Mutable Document Operations

<!-- example:readme_mutable_ops -->
```cpp
// Create mutable document object, default construction also creates empty {} root node
yyjson::MutableDocument mutDoc("{}"); // Special literal represents empty object

// Cannot use path operator / to add new keys, index operator [] supports auto-addition
mutDoc["name"] = "Bob";
mutDoc["scores"] = "[]"; // Special literal represents empty array

// Array appending
mutDoc / "scores" << 95 << 87;

// File writing
mutDoc.writeFile("output.json");

// Standard stream output: {"name":"Bob","scores":[95,87]}
std::cout << mutDoc << std::endl;
```

### Iteration

<!-- example:readme_iterator -->
```cpp
yyjson::Document doc(R"({"user":{"name":"Alice", "age":"30"}, "items": ["apple","banana","cherry"]})");

// Array iteration, iter.value() can be simplified to dereference *iter
for (auto iter = doc / "items" % 0; iter; ++iter) {
    std::cout << "Item " << iter.index() << ": " << (iter.value() | "") << std::endl;
}

// Object iteration, iter.value() can be simplified to dereference *iter
for (auto iter = doc / "user" % ""; iter; ++iter) {
    std::cout << iter.name() << " = " << (iter.value() | "") << std::endl;
}
```

## Project Status

- ✅ **Stable and Usable** - Core functionality completed and tested
- 🔄 **Continuous Development** - Gradually improving features according to [requirements list](task_todo.md)
- 🧪 **Test Coverage** - Comprehensive unit tests ensure quality
- ✅ **Continuous Integration** - Configured [Actions](https://github.com/lymslive/xyjson/actions) CI/CD pipeline

## Development Workflow

See [Development Guide](DEVELOPMENT_GUIDE.md) for details.

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
├── examples/            # Application examples (no dependency on test framework)
├── perf/                # Performance tests (mainly compared with native yyjson)
├── docs/                # Detailed documentation
├── task_todo.md         # Development requirements management
├── task_log.md          # Task completion records
└── CMakeLists.txt       # Build configuration (supports find_package)
```

### Documentation Navigation

- 📖 [Usage Guide](docs/usage.md) - Detailed usage tutorials and best practices
- 🔧 [API Reference](docs/api.md) - Complete operator and class method documentation
- 🎨 [Design Philosophy](docs/design.md) - Library design ideas and philosophy
- 🧪 [Unit Tests](utest/README.md) - Unit tests documentation
- 🧪 [Performance Tests](perf/README.md) - Performance tests documentation
- 🧪 [Application Examples](examples/README.md) - Application use case documentation
- 📋 [Development Requirements](task_todo.md) - Current project development requirements list
- 📊 [Task Log](task_log.md) - AI collaboration task completion records

## Contributing

Welcome to submit Issues and Pull Requests!

## License

MIT License - See [LICENSE](LICENSE) file for details

## Related Projects

- [yyjson](https://github.com/ibireme/yyjson) - Underlying high-performance JSON library
- [couttast](https://github.com/lymslive/couttast) - Unit testing framework

---

*Simple and intuitive JSON operations, making C++ development more enjoyable!*