# Find the XQilla library
#
# originally taken from
#  https://github.com/rug-compling/alpinocorpus/blob/master/cmake/FindXQilla.cmake

find_path(XQILLA_INCLUDE_DIR NAMES xqilla/xqilla-simple.hpp)

find_library(XQILLA_LIBRARY NAMES xqilla)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(XQILLA DEFAULT_MSG
      XQILLA_INCLUDE_DIR XQILLA_LIBRARY)

set(XQILLA_LIBRARIES ${XQILLA_LIBRARY})

mark_as_advanced(XQILLA_INCLUDE_DIR XQILLA_LIBRARY)

if (NOT TARGET XQilla)
    add_library(XQilla UNKNOWN IMPORTED)
    if (XQILLA_INCLUDE_DIR)
        set_target_properties(XQilla PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${XQILLA_INCLUDE_DIR}")
    endif()
endif()
