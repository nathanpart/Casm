add_library(parser "")

target_sources(parser
    PRIVATE
        graminit.cpp
        grammar.cpp
        node.cpp
        Parser.cpp
        token.cpp
        tokenizer.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/errcode.h
        ${CMAKE_CURRENT_LIST_DIR}/graminit.h
        ${CMAKE_CURRENT_LIST_DIR}/grammar.h
        ${CMAKE_CURRENT_LIST_DIR}/node.h
        ${CMAKE_CURRENT_LIST_DIR}/Parser.h
        ${CMAKE_CURRENT_LIST_DIR}/token.h
        ${CMAKE_CURRENT_LIST_DIR}/tokenizer.h
)

target_include_directories(parser
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)