# xyjson

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[English Version](README-en.md) | [中文文档](README.md)

xyjson is a header-only C++ wrapper library built on the high-performance [yyjson](https://github.com/ibireme/yyjson). It provides an intuitive JSON handling experience via operator overloading.

Meaning of the project name:
- The library is based on yyjson. The wrapped C++ classes act as proxies for the corresponding yyjson structures (hence the proxy/"xy" implication), and the namespace continues to use `yyjson::`.
- The symbols **x** and **y** are commonly used in mathematics; xyjson aims to let you operate on JSON data as conveniently as you would with mathematical variables.

## Features

- 🚀 **High performance** — built on yyjson with a zero-copy design
- ✨ **Intuitive syntax** — rich operator overloads, similar to native C++
- 🔒 **Type safe** — compile-time checks and safe runtime extraction
- 📚 **Full functionality** — supports read/write, iteration, file operations, etc.
- 🛠️ **Easy to integrate** — header-only, with CMake support and `find_package` integration

## Dependencies

- **[yyjson](https://github.com/ibireme/yyjson)** — core dependency, high-performance JSON parser
- **[couttast](https://github.com/lymslive/couttast)** — optional, used for testing and development only
- **C++ standard**: C++17 or newer
- **Platforms**: Linux, macOS, Windows (MinGW)

## Quick start

### Use without installation

If you have the underlying yyjson library available, simply copy the single header file `include/xyjson.h` into your project and use it directly.

**Code example:**
<!-- example:readme_quick_start -->
```cpp
#include "xyjson.h"

// Read JSON
std::string json = R"({"name": "Alice", "age": 30})";
yyjson::Document doc(json);

// Extract values
std::string name = doc / "name" | ""; // reads "Alice"
int age = doc / "age" | 0;            // reads 30
```

### CMake integration / Install

Supports a standard CMake build flow:

```bash
# Clone project
git clone https://github.com/lymslive/xyjson
cd xyjson

# Build (dependencies will be auto-downloaded if not installed)
mkdir build && cd build
cmake .. && make

# Install to system directories
sudo make install
```

By default the project performs a full build: unit tests and examples are built, but performance tests are not included unless you enable them with `-DBUILD_PERF=ON`.

#### Minimal build and custom install

If you only need the library as a dependency, you can skip tests and examples by enabling `-DXYJSON_LIB_ONLY=ON` when configuring CMake. If you don't have permission to write to system directories, set `CMAKE_INSTALL_PREFIX` to a directory you control (for example `$HOME`).

```bash
# Build only the library and skip tests/examples
mkdir build && cd build
cmake .. -DXYJSON_LIB_ONLY=ON -DCMAKE_INSTALL_PREFIX=$HOME && make

# Install to $HOME
make install
```

#### Consume xyjson from other projects

After installation you can use `find_package` in downstream projects:

```cmake
# In your CMakeLists.txt
find_package(xyjson REQUIRED)
target_link_libraries(your-target PRIVATE xyjson::xyjson)
```

If you prefer not to pre-install xyjson, you can use FetchContent to pull it at configure time. It is recommended to pass `-DXYJSON_LIB_ONLY=ON` to avoid building tests and examples unnecessarily:

```cmake
include(FetchContent)

FetchContent_Declare(
    xyjson
    GIT_REPOSITORY https://github.com/lymslive/xyjson.git
    GIT_TAG main
    # Build only the library, skip tests and examples (recommended)
    OPTIONS "-DXYJSON_LIB_ONLY=ON"
)

FetchContent_MakeAvailable(xyjson)
target_link_libraries(your-target PRIVATE xyjson::xyjson)
```

For more details see the FetchContent usage guide: [docs/fetchcontent_usage.md](docs/fetchcontent_usage.md)

## Core usage examples

### Basic operations

<!-- example:readme_basic_ops -->
```cpp
// Create a document from a JSON string; you can also parse by doing doc << json_string
yyjson::Document doc(R"({"name": "Alice", "scores": [95, 87]})");

// Path access
std::string name = doc / "name" | ""; // "Alice"

// Array access
int firstScore = doc / "scores" / 0 | 0; // 95

// Type checks
bool isString = doc / "name" & ""; // true
bool isNumber = doc / "scores" / 0 & 0;   // true
```

### Mutable document operations

<!-- example:readme_mutable_ops -->
```cpp
// Create a mutable document; default construction also creates an empty {} root
yyjson::MutableDocument mutDoc("{}"); // special literal denotes empty object

// You cannot add new keys with the path operator /; use the index operator [] which supports auto-add
mutDoc["name"] = "Bob";
mutDoc["scores"] = "[]"; // special literal denotes empty array

// Append to array
mutDoc / "scores" << 95 << 87;

// Write to file
mutDoc.writeFile("output.json");

// Stream output: {"name":"Bob","scores":[95,87]}
std::cout << mutDoc << std::endl;
```

### Iteration

<!-- example:readme_iterator -->
```cpp
yyjson::Document doc(R"({"user":{"name":"Alice", "age":"30"}, "items": ["apple","banana","cherry"]})");

// Array iteration; iter.value() can be simplified by dereferencing *iter
for (auto iter = doc / "items" % 0; iter; ++iter) {
    std::cout << "Item " << iter.index() << ": " << (iter.value() | "") << std::endl;
}

// Object iteration; iter.value() can be simplified by dereferencing *iter
for (auto iter = doc / "user" % ""; iter; ++iter) {
    std::cout << iter.name() << " = " << (iter.value() | "") << std::endl;
}
```

## Project status

- ✅ **Stable and usable** — core functionality is complete and tested
- 🔄 **Active development** — features are continuously improved per [task list](task_todo.md)
- 🧪 **Test coverage** — comprehensive unit tests ensure quality
- ✅ **CI** — GitHub Actions CI/CD is configured: https://github.com/lymslive/xyjson/actions

## Development workflow

See the [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) for details.

### Running tests

```bash
cd build
./utxyjson --cout=silent  # silent mode
./utxyjson                # verbose output
```

### Code layout

```
xyjson/
├── include/xyjson.h     # main header (header-only library)
├── utest/               # unit tests (with rich examples)
├── examples/            # application examples (no dependency on test framework)
├── perf/                # performance tests (mainly compared with native yyjson)
├── docs/                # detailed documentation
├── task_todo.md         # development requirements
├── task_log.md          # task completion records
└── CMakeLists.txt       # build configuration (supports find_package)
```

### Documentation navigation

- 📖 [Usage guide](docs/usage.md) - detailed tutorials and best practices
- 🔧 [API reference](docs/api.md) - full operator and class method documentation
- 🎨 [Design notes](docs/design.md) - library design and philosophy
- 🧪 [Unit tests](utest/README.md) - unit test documentation
- 🧪 [Performance tests](perf/README.md) - performance test documentation
- 🧪 [Examples](examples/README.md) - application examples
- 📋 [Development requirements](task_todo.md) - current project tasks
- 📊 [Task log](task_log.md) - AI collaboration task records

## Contributing

Contributions via Issues and Pull Requests are welcome.

## License

MIT License — see [LICENSE](LICENSE) for details

## Related projects

- [yyjson](https://github.com/ibireme/yyjson) - underlying high-performance JSON library
- [couttast](https://github.com/lymslive/couttast) - unit test framework

---

*Concise and intuitive JSON operations to make C++ development more pleasant.*