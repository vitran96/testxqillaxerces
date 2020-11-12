
set(XQilla_VERSION_MAJOR "2")
set(XQilla_VERSION_MINOR "3")

if (NOT TARGET XQilla::XQilla AND
    NOT ${XQilla_ROOT} STREQUAL "" AND
    EXISTS ${XQilla_ROOT})
    LIST(APPEND CMAKE_PREFIX_PATH ${XQilla_ROOT})
endif ()

find_package(XQilla REQUIRED)
