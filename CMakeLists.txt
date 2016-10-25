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
elseif(APPLE)
	message(STATUS "Using Apple")
	set(ADDITIONAL_LIBS curses z)
	link_directories(${LLVM_DIR}/lib)
	add_definitions(-DLEADING_UNDERSCORE)
else() # Linux
	message(STATUS "Using Linux")
	#set(ADDITIONAL_LIBS curses z)
	set(ADDITIONAL_LIBS pthread dl)
	link_directories(${LLVM_DIR}/build/lib)
	add_custom_target(t COMMAND ./shmoptix ../matte.sl DEPENDS shmoptix)
	add_custom_target(e COMMAND vi ../shmoptix.cc)
	add_custom_target(gdb COMMAND gdb ./shmoptix)
	#add_definitions(-DLEADING_UNDERSCORE)
endif()

add_executable(shmoptix shmoptix.cc global.h AST.h CodeGen.h ExecutionEnvironment.h Lexer.h ErrorHandler.h Parser.h)

if(MSVC)
    set(VS_STARTUP_PROJECT shmoptix)
endif()


if(MSVC)
    set(LLVM_CONFIG ${LLVM_DIR}/build/Debug/bin/llvm-config${EXE})
elseif(APPLE)
    set(LLVM_CONFIG ${LLVM_DIR}/bin/llvm-config${EXE})
else() # Linux
    set(LLVM_CONFIG ${LLVM_DIR}/build/bin/llvm-config${EXE} )
endif()

set(LLVM_MODULES Core ExecutionEngine Interpreter MC MCJIT Support nativecodegen)
#set(LLVM_MODULES all)

execute_process(COMMAND ${LLVM_CONFIG} --libnames ${LLVM_MODULES} OUTPUT_VARIABLE LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
separate_arguments(LLVM_LIBS)

set(LIBS ${LLVM_LIBS} ${ADDITIONAL_LIBS})

target_link_libraries(shmoptix ${LIBS})
