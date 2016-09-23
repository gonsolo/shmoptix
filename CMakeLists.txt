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

add_executable(show show.cc global.h AST.h CodeGen.h Lexer.h ErrorHandler.h Parser.h)

target_link_libraries(show
	LLVMCore LLVMSupport LLVMCodeGen LLVMAnalysis LLVMMC LLVMSelectionDAG LLVMTarget
	LLVMX86CodeGen LLVMX86Info LLVMX86Desc LLVMX86Utils LLVMX86AsmPrinter 
	LLVMMCDisassembler LLVMScalarOpts LLVMTransformUtils LLVMAsmParser LLVMAsmPrinter
	LLVMMCParser LLVMDebugInfoCodeView LLVMExecutionEngine)
if(APPLE)
    target_link_libraries(show curses)
endif()

add_custom_target(t COMMAND ./show DEPENDS show)
add_custom_target(e COMMAND vi ../show.cc)
add_custom_target(gdb COMMAND gdb ./show)
