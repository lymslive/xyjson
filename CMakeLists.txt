cmake_minimum_required(VERSION 3.15)
project(xyjson VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compile options for Debug build
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Add $HOME to search paths for some dependent libs may install there
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}")

# Try to find packages first, fallback to FetchContent if not found
option(USE_FETCHCONTENT "Use FetchContent for dependencies if not found" ON)

# Option to build only the interface library (skip tests and examples)
option(XYJSON_LIB_ONLY "Build only the header-only interface library (skip tests and examples)" OFF)

# Add FetchContent module if CMake version supports it
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.14 AND USE_FETCHCONTENT)
    include(FetchContent)
endif()

# Try to find yyjson
find_package(yyjson QUIET)
if(NOT yyjson_FOUND)
    if(USE_FETCHCONTENT AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.14)
        message(STATUS "yyjson not found, using FetchContent...")
        FetchContent_Declare(
            yyjson
            GIT_REPOSITORY https://github.com/ibireme/yyjson.git
            GIT_TAG master
        )
        FetchContent_MakeAvailable(yyjson)
    else()
        message(FATAL_ERROR "yyjson not found and FetchContent not available/disabled")
    endif()
else()
    message(STATUS "Found yyjson via find_package")
endif()


# Create interface library for header-only xyjson
add_library(xyjson INTERFACE)
target_include_directories(xyjson INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(xyjson INTERFACE yyjson)

# Install headers only for header-only library
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Install CMake configuration files
install(FILES
    cmake/xyjsonConfig.cmake
    cmake/xyjsonConfigVersion.cmake
    DESTINATION lib/cmake/xyjson
)

# Export the target for use from the install tree
install(TARGETS xyjson
    EXPORT xyjsonTargets
    INCLUDES DESTINATION include
)

# Install the export set
install(EXPORT xyjsonTargets
    FILE xyjsonTargets.cmake
    NAMESPACE xyjson::
    DESTINATION lib/cmake/xyjson
)

# ------------------------------------------------------------ #
# Try to find couttast (needed for tests and performance tests)
if(NOT XYJSON_LIB_ONLY)
    find_package(couttast QUIET)
    if(NOT couttast_FOUND)
        if(USE_FETCHCONTENT AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.14)
            message(STATUS "couttast not found, using FetchContent...")
            FetchContent_Declare(
                couttast
                GIT_REPOSITORY https://github.com/lymslive/couttast.git
                GIT_TAG main
            )
            FetchContent_MakeAvailable(couttast)
        else()
            message(FATAL_ERROR "couttast not found and FetchContent not available/disabled")
        endif()
    else()
        message(STATUS "Found couttast via find_package")
    endif()
endif()

# ------------------------------------------------------------ #
# Unit Test, Examples, and Performance Test (only if not lib-only mode)

if(NOT XYJSON_LIB_ONLY)
    # Unit Test:
    add_subdirectory(utest)

    # Examples:
    add_subdirectory(examples)

    # Performance Test:
    option(BUILD_PERF "Build performance tests" OFF)
    if(BUILD_PERF)
        add_subdirectory(perf)
    endif()
else()
    message(STATUS "XYJSON_LIB_ONLY enabled: skipping tests, examples, and performance tests")
endif()

