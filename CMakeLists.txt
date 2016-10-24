cmake_minimum_required(VERSION 3.5)

project(Shmoptix)

add_compile_options(-std=c++14)

set(LLVM_DIR CACHE PATH "The location of the LLVM installation")
include_directories(${LLVM_DIR}/include)
include_directories(${LLVM_DIR}/build/include)

if(MSVC)
    message(STATUS "Using MSVC")
    link_directories(${LLVM_DIR}/build/Debug/lib)
	set(EXE .exe)
endif()
if(APPLE)
    message(STATUS "Using Apple")
	set(ADDITIONAL_LIBS curses z)
	link_directories(${LLVM_DIR}/lib)
	add_custom_target(t COMMAND ./shmoptix DEPENDS shmoptix)
	add_custom_target(e COMMAND vi ../shmoptix.cc)
	add_custom_target(gdb COMMAND gdb ./shmoptix)
endif()

add_executable(shmoptix shmoptix.cc global.h AST.h CodeGen.h ExecutionEnvironment.h Lexer.h ErrorHandler.h Parser.h)

if(MSVC)
    set(VS_STARTUP_PROJECT shmoptix)
endif()

set(LLVM_MODULES Core ExecutionEngine Interpreter MC MCJIT Support nativecodegen)

if(MSVC)
    set(LLVM_CONFIG ${LLVM_DIR}/build/Debug/bin/llvm-config${EXE} )
endif()
if(APPLE)
    set(LLVM_CONFIG ${LLVM_DIR}/bin/llvm-config${EXE} )
endif()

execute_process(COMMAND ${LLVM_CONFIG} --libnames ${LLVM_MODULES} OUTPUT_VARIABLE LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
separate_arguments(LLVM_LIBS)

set(LIBS ${LLVM_LIBS} ${ADDITIONAL_LIBS})

target_link_libraries(shmoptix ${LIBS})