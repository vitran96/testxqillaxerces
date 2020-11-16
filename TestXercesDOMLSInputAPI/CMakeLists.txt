project("TestXercesDOMLSInputAPI")

add_executable(TestXercesDOMLSInputAPI "testdomlsinput.cpp" "testdomlsinput.h")

target_link_libraries(${PROJECT_NAME}
    XercesC::XercesC
    XQilla::XQilla
)
