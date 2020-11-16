project("TestXqilla")

add_executable(TestXqilla "testxqilla.cpp" "testxqilla.h")

target_link_libraries(${PROJECT_NAME}
    XercesC::XercesC
    XQilla::XQilla
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${XercesC_BIN}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${XQilla_BIN}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/")