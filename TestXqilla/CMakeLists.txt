project("TestXqilla")

add_executable(TestXqilla "testxqilla.cpp" "testxqilla.h")

#message(${XQilla})
#message(${XQILLA_LIBRARY})

target_link_libraries(${PROJECT_NAME}
    XercesC::XercesC
    XQilla
)
