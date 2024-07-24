cmake_minimum_required(VERSION 3.10)
project(MyPythonLikeCompiler)

set(CMAKE_C_STANDARD 11)

# Include the directory containing header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
add_executable(my_compiler
    src/lexer.c
    src/parser.c
    src/ast.c
    src/ir_generation.c
    src/main.c
)

add_executable(test_parser
    src/lexer.c
    src/parser.c
    src/ast.c
    test/test_parser.c
)
