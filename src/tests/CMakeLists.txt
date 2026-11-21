add_executable(snake_tests
    test_game_logic.cpp
)

target_link_libraries(snake_tests PRIVATE
    snake_core
    GTest::gtest_main
)

target_include_directories(snake_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
)

include(GoogleTest)
gtest_discover_tests(snake_tests)