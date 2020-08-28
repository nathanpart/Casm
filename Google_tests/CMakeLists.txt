project(Google_tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include {gtest_SOURCE_DIR})

add_executable(Google_Tests_run ParserTest.cpp TokenizerTest.cpp)

target_link_libraries(Google_Tests_run assembler parser)

target_link_libraries(Google_Tests_run gtest gtest_main)