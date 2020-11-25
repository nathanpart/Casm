add_subdirectory(Parser)
add_subdirectory(assembler)

add_executable(casm casm.cpp)

target_link_libraries(casm
        PUBLIC
        parser
        assembler
        )


add_executable(dump_nodes dump_nodes.cpp)

target_link_libraries(dump_nodes
        PUBLIC
        parser
        assembler
        )
