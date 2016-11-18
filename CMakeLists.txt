cmake_minimum_required(VERSION 3.5)

project(Shmoptix)

find_package(LLVM REQUIRED CONFIG)

# Set LLVM_DIR to <llvm_dir>/build/cmake/modules/CMakeFiles
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_compile_options(-std=c++14)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${LLVM_INCLUDE_DIRS}/../../include) # bug?
add_definitions(${LLVM_DEFINITIONS})

message(STATUS ${LLVM_INCLUDE_DIRS})

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

if(MSVC)
	message(STATUS "Using MSVC")
	# Suppress warnings like LLVM/cmake/modules/HandleLLVLMOptions.cmake
	add_definitions("/wd4141 /wd4146 /wd4267 /wd4244 /wd4800 /wd4291")

	include_directories(${LLVM_DIR}/include)
	include_directories(${LLVM_DIR}/build/include)
	link_directories(${LLVM_DIR}/build/Debug/lib)
	set(EXE .exe)
elseif(APPLE)
	message(STATUS "Using Apple")
	set(ADDITIONAL_LIBS curses z)
	include_directories(${LLVM_DIR}/include)
	link_directories(${LLVM_DIR}/lib)
	add_definitions(-DLEADING_UNDERSCORE)
elseif(LINUX)
	message(STATUS "Using Linux")
	set(ADDITIONAL_LIBS pthread dl)
	include_directories(${LLVM_DIR}/include)
	link_directories(${LLVM_DIR}/lib)
	add_custom_target(t COMMAND ./shmoptix ../matte.sl DEPENDS shmoptix)
	add_custom_target(e COMMAND vi ../shmoptix.cc)
	add_custom_target(gdb COMMAND gdb ./shmoptix)
endif()

add_executable(shmoptix shmoptix.cc color.h global.h AST.h CodeGen.h ExecutionEnvironment.h Lexer.h ErrorHandler.h Parser.h)

if(MSVC)
    set(VS_STARTUP_PROJECT shmoptix)
endif()

llvm_map_components_to_libnames(llvm_libs Core ExecutionEngine Interpreter MC MCJIT Support nativecodegen)
target_link_libraries(shmoptix ${llvm_libs})
