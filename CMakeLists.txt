cmake_minimum_required(VERSION 3.10)
project(TypeSafePythonLikeLanguage)

set(CMAKE_C_STANDARD 11)

# Include the directory containing header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
add_executable(test_lexer
    src/lexer.c
    test/test_lexer.c
)
