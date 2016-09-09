cmake_minimum_required(VERSION 3.5)

project(Show)

add_compile_options(-std=c++14)

add_executable(show show.cc global.h AST.h Lexer.h ErrorHandler.h Parser.h)

add_custom_target(t COMMAND ./show DEPENDS show)
add_custom_target(e COMMAND vi ../show.cc)
add_custom_target(gdb COMMAND gdb ./show)

