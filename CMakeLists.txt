cmake_minimum_required(VERSION 3.5)

project(Show)

add_compile_options(-std=c++14)

set(LLVM_DIR CACHE PATH "The location of the LLVM installation")
include_directories(${LLVM_DIR}/include)
include_directories(${LLVM_DIR}/build/include)
link_directories(${LLVM_DIR}/build/Debug/lib)

add_executable(show show.cc global.h AST.h Lexer.h ErrorHandler.h Parser.h)
target_link_libraries(show LLVMCore LLVMSupport)

add_custom_target(t COMMAND ./show DEPENDS show)
add_custom_target(e COMMAND vi ../show.cc)
add_custom_target(gdb COMMAND gdb ./show)

