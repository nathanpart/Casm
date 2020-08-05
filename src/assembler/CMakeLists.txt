add_subdirectory(instructions)
add_library(assembler "")

target_sources(assembler
    PRIVATE
        Line.cpp
        Assembler.cpp
        Label.cpp
        Expression.cpp
        Segment.cpp
        AddressMode.cpp
        Instruction.cpp
        AsmState.cpp
        PseudoOp.cpp
        Variable.cpp
        Region.cpp
        MacroLine.cpp
        Macro.cpp
        Location.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Line.h
        ${CMAKE_CURRENT_LIST_DIR}/Assembler.h
        ${CMAKE_CURRENT_LIST_DIR}/Label.h
        ${CMAKE_CURRENT_LIST_DIR}/ExportImport.h
        ${CMAKE_CURRENT_LIST_DIR}/Expression.h
        ${CMAKE_CURRENT_LIST_DIR}/Symbols.h
        ${CMAKE_CURRENT_LIST_DIR}/Segment.h
        ${CMAKE_CURRENT_LIST_DIR}/AddressMode.h
        ${CMAKE_CURRENT_LIST_DIR}/Instruction.h
        ${CMAKE_CURRENT_LIST_DIR}/AsmState.h
        ${CMAKE_CURRENT_LIST_DIR}/PseudoOp.h
        ${CMAKE_CURRENT_LIST_DIR}/Variable.h
        ${CMAKE_CURRENT_LIST_DIR}/Region.h
        ${CMAKE_CURRENT_LIST_DIR}/MacroLine.h
        ${CMAKE_CURRENT_LIST_DIR}/Macro.h
        ${CMAKE_CURRENT_LIST_DIR}/Location.h

)

target_include_directories(assembler
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)