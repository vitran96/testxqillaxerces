cmake_minimum_required (VERSION 3.8)

project("TestXpathWithXQilla")

#Using /std:c++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#include(FindXercesC)
include(FindXQilla)
include(AddXercesC)
include(AddXQilla)

# Include sub-projects.
add_subdirectory ("TestXqilla")

message(STATUS "  XQilla library:            ${XQilla_LIBRARIES}")
message(STATUS "  XQilla header folder:      ${XQilla_INCLUDE_DIRS}")
message(STATUS "  XQilla binary:             ${XQilla_BIN}")
message("-----------------------------")
message(STATUS "  XercesC library:           ${XercesC_LIBRARIES}")
message(STATUS "  XercesC header folder:     ${XercesC_INCLUDE_DIRS}")
message(STATUS "  Xerces binary:             ${XercesC_BIN}")
message("-----------------------------")
message(STATUS "  C compiler:                ${CMAKE_C_COMPILER}")
message(STATUS "  C++ compiler:              ${CMAKE_CXX_COMPILER}")
message(STATUS "  C++ flags:                 ${CMAKE_CXX_FLAGS}")
