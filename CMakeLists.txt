cmake_minimum_required (VERSION 3.8)

#Using /std:c++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH
         "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(FindXQilla)

find_package(XercesC 3.1 REQUIRED)
find_package(XQilla REQUIRED)

# Include sub-projects.
add_subdirectory ("TestXqilla")

