project("TestXqilla")

add_executable(TestXqilla "testxqilla.cpp" "testxqilla.h")

target_link_libraries(${PROJECT_NAME} XercesC::XercesC)

