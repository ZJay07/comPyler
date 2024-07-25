cmake_minimum_required(VERSION 3.10)
project(MyPythonLikeCompiler)

set(CMAKE_C_STANDARD 11)

# Include the directory containing header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files for the main compiler
add_executable(my_compiler
    src/lexer.c
    src/parser.c
    src/ast.c
    src/symbol_table.c
    src/semantic_analysis.c
    src/ir_generation.c
    src/main.c
)

# Add source files for the parser test
add_executable(test_parser
    src/lexer.c
    src/parser.c
    src/ast.c
    test/test_parser.c
)

# Add source files for semantic analysis test
add_executable(test_semantic_analysis
    src/lexer.c
    src/parser.c
    src/ast.c
    src/symbol_table.c 
    src/semantic_analysis.c
    test/test_semantic_analysis.c
)
