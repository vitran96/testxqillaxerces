project("TestXqilla")

add_executable(TestXqilla "testxqilla.cpp" "testxqilla.h")

message(${xqilla})
message(${XQILLA_LIBRARY})

target_link_libraries(${PROJECT_NAME} XercesC::XercesC xqilla)

