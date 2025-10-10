# AI Agents Guide for xyjson

This file provides essential information for AI agents to work effectively in the xyjson codebase.

## Build Commands

### Standard Build
```bash
mkdir -p build && cd build
cmake .. && make
```

### Testing
```bash
# Run all tests (silent mode)
./build/utxyjson --cout=silent

# Run with detailed output
./build/utxyjson
```

### Clean Build
```bash
rm -rf build && mkdir build && cd build && cmake .. && make
```

## Architecture Overview

### Core Design
xyjson is a C++ wrapper library around the yyjson C library, providing operator-overloaded JSON manipulation. The library maintains two parallel hierarchies:

- **Read-only**: `Document` → `Value` with `ArrayIterator`/`ObjectIterator`
- **Mutable**: `MutableDocument` → `MutableValue` with `MutableArrayIterator`/`MutableObjectIterator`

### Key Implementation Patterns

1. **Operator Overloading**: Core functionality is exposed through operators (`/`, `|`, `=`, `<<`, `%`, etc.)
2. **RAII Memory Management**: Documents automatically manage yyjson memory allocation
3. **Zero-Copy Design**: Leverages yyjson's performance by operating on original data
4. **Type Safety**: Compile-time checks with safe runtime value extraction

### File Structure Significance

- `include/xyjson.h`: Single-header implementation (both declarations and inline implementations)
- `src/xyjson.cpp`: Out-of-line implementations for larger methods
- `utest/t_xyjson.cpp`: Comprehensive test suite using couttast framework

### Dependencies Configuration

- **yyjson**: System-installed (`/usr/local/include/yyjson.h`)
- **couttast**: User-local test framework (`~/include/couttast/`, `~/lib/libcouttast.a`)
- **C++17**: Required for modern features used in implementation

### Important Implementation Details

- **Namespace**: `yyjson` (intentionally kept to honor the underlying library)
- **Error Handling**: Boolean operators for validity checks (`if (doc)`, `if (!value)`)
- **String Optimization**: Literal strings use reference optimization, dynamic strings use safe copying
- **Path Resolution**: `/` operator supports both chained and JSON Pointer syntax

### Testing Philosophy

The test suite (`t_xyjson.cpp`) serves as both:
1. **Validation**: Ensuring functionality works correctly
2. **Documentation**: Demonstrating usage patterns through examples

Tests are organized by functionality groups and use the couttast framework's `DEF_TAST` macros.