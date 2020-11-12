
if (NOT TARGET XercesC::XercesC AND
    NOT ${XercesC_ROOT} STREQUAL "" AND
    EXISTS ${XercesC_ROOT})
    LIST(APPEND CMAKE_PREFIX_PATH ${XercesC_ROOT})
endif ()

find_package(XercesC 3.1 REQUIRED)
