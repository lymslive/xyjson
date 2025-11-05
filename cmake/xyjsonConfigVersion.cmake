# Package version configuration file for xyjson

# Check if the requested version is compatible
set(PACKAGE_VERSION "1.0.0")

# Check if the requested version matches
if("${PACKAGE_FIND_VERSION}" VERSION_EQUAL "${PACKAGE_VERSION}")
    set(PACKAGE_VERSION_COMPATIBLE TRUE)
    set(PACKAGE_VERSION_EXACT TRUE)
elseif("${PACKAGE_FIND_VERSION}" VERSION_LESS "${PACKAGE_VERSION}")
    set(PACKAGE_VERSION_COMPATIBLE TRUE)
    set(PACKAGE_VERSION_EXACT FALSE)
else()
    set(PACKAGE_VERSION_COMPATIBLE FALSE)
    set(PACKAGE_VERSION_EXACT FALSE)
endif()