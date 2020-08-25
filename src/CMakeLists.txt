add_subdirectory(Parser)
add_subdirectory(assembler)

add_executable(casm casm.cpp)


target_link_libraries(casm
        PUBLIC
        parser
        assembler
        )