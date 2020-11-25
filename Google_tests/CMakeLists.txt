project(Google_tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include {gtest_SOURCE_DIR})
include_directories(${gmock_SOURCE_DIR}/include {gmock_SOURCE_DIR})

add_executable(Google_Tests_run
        ParserTest.cpp
        TokenizerTest.cpp
        SourceItemTest.cpp
        LabelTest.cpp
        AsmTestMoc.h
        ExpressionTest.cpp
        )

target_link_libraries(Google_Tests_run assembler parser)

target_link_libraries(Google_Tests_run gtest gmock gtest_main)