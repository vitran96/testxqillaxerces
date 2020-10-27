project("TestXquilla")

add_executable(TestXquilla "TestXquilla.cpp" "TestXquilla.h")

target_link_libraries(${PROJECT_NAME} XercesC::XercesC)

