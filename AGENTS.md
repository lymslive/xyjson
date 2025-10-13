# AI Agents Guide for xyjson

This file provides essential information for AI agents to work effectively in the xyjson codebase.

## Project Overview

xyjson is a C++ wrapper library around the yyjson C library, providing operator-overloaded JSON manipulation. The project is now a **header-only library** with CMake integration support.

## Build Commands

### Standard Build
```bash
mkdir -p build && cd build
cmake .. && make
```

### Testing
```bash
# Run all tests (silent mode)
./utxyjson --cout=silent

# Run with detailed output
./utxyjson
```

### Clean Build
```bash
rm -rf build && mkdir build && cd build && cmake .. && make
```

## Key Project Changes

### Header-Only Library
- **Single header**: `include/xyjson.h` contains complete implementation
- **CMake integration**: Supports `find_package(xyjson REQUIRED)`
- **No compilation required**: Users can simply copy `xyjson.h` to their project

### Updated Dependency Management
- **System packages**: First tries to find `yyjson` and `couttast` via system installation
- **Auto-download**: Falls back to FetchContent if dependencies not found
- **Flexible**: Works with both system-installed and auto-downloaded dependencies

## Architecture Overview

### Core Design
xyjson maintains two parallel hierarchies:
- **Read-only**: `Document` → `Value` with `ArrayIterator`/`ObjectIterator`
- **Mutable**: `MutableDocument` → `MutableValue` with `MutableArrayIterator`/`MutableObjectIterator`

### Key Implementation Patterns

1. **Operator Overloading**: Core functionality via operators (`/`, `|`, `=`, `<<`, `%`, etc.)
2. **RAII Memory Management**: Automatic yyjson memory management
3. **Zero-Copy Design**: Leverages yyjson's performance
4. **Type Safety**: Compile-time checks with safe runtime value extraction

## File Structure

### Core Files
- `include/xyjson.h` - Single-header implementation with structured organization
  - **Table of Content**: Auto-generated TOC for navigation
  - **Part 1**: Forward declarations
  - **Part 2**: Class definitions (read-only/mutable models, iterators, type traits)
  - **Part 3**: Non-class functions (yyjson helper functions, conversion helpers)
  - **Part 4**: Class implementations (methods organized by functionality)
  - **Part 5**: Operator interface
  - **Part 6**: Final definitions

### Test Suite
- `utest/` - Modular test suite using couttast framework
  - `t_basic.cpp` - Basic functionality tests
  - `t_mutable.cpp` - Mutable document operations
  - `t_stream.cpp` - Stream operations
  - `t_iterator.cpp` - Iterator functionality
  - `t_conversion.cpp` - Type conversion
  - `t_advanced.cpp` - Advanced features
  - `t_experiment.cpp` - Experimental features
  - `README.md` - More details about unit test

### Build Configuration
- `CMakeLists.txt` - Main build configuration with dependency management
- `cmake/` - CMake configuration files for `find_package` integration

## Dependencies Configuration

### Primary Dependencies
- **yyjson**: System-installed or auto-downloaded via FetchContent
- **couttast**: Test framework, user-local or auto-downloaded

### Build Requirements
- **C++17**: Required for modern features
- **CMake 3.15+**: For FetchContent support

## Important Implementation Details

### Namespace and Design
- **Namespace**: `yyjson` (honors underlying library)
- **Error Handling**: Boolean operators for validity checks (`if (doc)`, `if (!value)`)
- **String Optimization**: Literal strings use reference optimization
- **Path Resolution**: `/` operator supports chained and JSON Pointer syntax

### Integration Methods
1. **Direct Copy**: Copy `include/xyjson.h` to project include directory
2. **CMake Integration**: Use `find_package(xyjson REQUIRED)` and `target_link_libraries()`

## Testing Philosophy

The modular test suite serves as both:
1. **Validation**: Ensuring functionality works correctly
2. **Documentation**: Demonstrating usage patterns through examples

Tests use couttast framework's `DEF_TAST` macros and can be run individually or as a group.

## Development Workflow

### Adding New Features
1. Implement in `include/xyjson.h` following existing patterns
2. Add comprehensive tests in appropriate `utest/t_*.cpp` file
3. Update documentation in `docs/` directory

### Testing Changes
- Always run `./utxyjson --cout=silent` after changes
- Use detailed output `./utxyjson {test_name}` for debugging
- Tests should pass in both silent and verbose modes

### Header File Organization
When modifying `xyjson.h`:
- Maintain the structured TOC organization
- Follow existing grouping patterns
- Update line numbers in TOC comments if sections are added/removed
- Use `script/gen_toc.pl` to regenerate TOC if needed

## Integration Guide

### For Applications
```cmake
# CMakeLists.txt
find_package(xyjson REQUIRED)
target_link_libraries(your-target PRIVATE xyjson::xyjson)
```

### For Development
```bash
# Clone and build
git clone <repository>
cd xyjson
mkdir build && cd build
cmake .. && make
# Run tests
./utxyjson --cout=silent
```

## AI Collaboration Guidelines

### Document Management
- `task_todo.md` - Original requirements (user-maintained, AI read-only)
- `task_log.md` - Task completion logs (AI-maintained)
- `DEVELOPMENT_GUIDE.md` - Detailed workflow and collaboration standards

### Working with Requirements
- When given a requirement ID (e.g., `TODO:2025-10-13/4`), refer to `task_todo.md` for details
- After completing work, update `task_log.md` and mark TODO as DONE in `task_todo.md`

### Git Commit Standards
- Use format: `<type>(scope): brief description`
- **scope**: Use requirement ID format `YYYY-MM-DD/n` when available
- **type**: Common types: `feat`, `fix`, `docs`, `refactor`, `style`, `perf`, `test`, `chore`
- Use English for prefixes, Chinese for descriptions

### Code Documentation
- **Code comments**: Use English, keep concise
- **Documentation**: Use Chinese for detailed explanations
- Group related overloaded functions with unified comments

This guide provides AI agents with the essential information needed to work effectively with the xyjson codebase after recent refactoring.

**Related Documents**:
- `DEVELOPMENT_GUIDE.md` - Detailed workflow and collaboration standards
- `task_todo.md` - Current development requirements
- `task_log.md` - Task completion history