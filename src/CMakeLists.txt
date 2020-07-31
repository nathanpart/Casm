add_executable(casm casm.cpp)

add_subdirectory(Parser)
add_subdirectory(assembler)

target_link_directories(casm
    PRIVATE
        parser
        assembler
)