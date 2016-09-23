cmake_minimum_required(VERSION 3.5)

project(Show)

add_compile_options(-std=c++14)

set(LLVM_DIR CACHE PATH "The location of the LLVM installation")
include_directories(${LLVM_DIR}/include)
include_directories(${LLVM_DIR}/build/include)
if(MSVC)
    link_directories(${LLVM_DIR}/build/Debug/lib)
endif()
if(APPLE)
link_directories(${LLVM_DIR}/lib)
endif()

add_executable(show show.cc global.h AST.h Lexer.h ErrorHandler.h Parser.h)

if(APPLE)
    target_link_libraries(show LLVMCore LLVMSupport curses)
else()
    target_link_libraries(show LLVMCore LLVMSupport)
endif()

add_custom_target(t COMMAND ./show DEPENDS show)
add_custom_target(e COMMAND vi ../show.cc)
add_custom_target(gdb COMMAND gdb ./show)
