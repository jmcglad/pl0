find_package(CLI11 REQUIRED)
find_package(fmt REQUIRED)
find_package(Git REQUIRED)

find_program(RE2C re2c)

execute_process(
    COMMAND ${GIT_EXECUTABLE} --no-pager log -n 1 --format=%H
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file("version.in.hpp" "version.hpp" @ONLY)

set(LEXER_IN ${CMAKE_CURRENT_SOURCE_DIR}/lexer.re.cpp)
set(LEXER_CPP ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)

add_custom_command(
    OUTPUT ${LEXER_CPP}
    COMMAND ${RE2C} -W -o ${LEXER_CPP} ${LEXER_IN}
    MAIN_DEPENDENCY ${LEXER_IN}
    VERBATIM CODEGEN
)

add_executable(pl0 ${LEXER_CPP} parser.cpp evaluator.cpp dot.cpp main.cpp)
target_compile_features(pl0 PRIVATE cxx_std_23)
target_include_directories(pl0 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(pl0 PRIVATE CLI11::CLI11 fmt::fmt)
