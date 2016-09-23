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

# Generated with llvm-config --libnames Core ExecutionEngine Interpreter MC MCJIT Support nativecodegen
target_link_libraries(show
LLVMX86CodeGen.lib LLVMX86Desc.lib LLVMX86Info.lib LLVMMCDisassembler.lib LLVMX86AsmPrinter.lib LLVMX86Utils.lib LLVMSelectionDAG.lib LLVMAsmPrinter.lib LLVMDebugInfoCodeView.lib LLVMMCJIT.lib LLVMInterpreter.lib LLVMCodeGen.lib LLVMScalarOpts.lib LLVMInstCombine.lib LLVMInstrumentation.lib LLVMTransformUtils.lib LLVMBitWriter.lib LLVMExecutionEngine.lib LLVMTarget.lib LLVMAnalysis.lib LLVMProfileData.lib LLVMRuntimeDyld.lib LLVMObject.lib LLVMMCParser.lib LLVMBitReader.lib LLVMMC.lib LLVMCore.lib LLVMSupport.lib
)

#target_link_libraries(show
#	LLVMCodeGen LLVMSelectionDAG 
#	LLVMX86CodeGen LLVMX86Info LLVMX86Desc LLVMX86Utils LLVMX86AsmPrinter 
#	LLVMMCDisassembler LLVMScalarOpts LLVMTransformUtils LLVMAsmParser LLVMAsmPrinter
#	LLVMDebugInfoCodeView 
#	LLVMInterpreter
#)

if(APPLE)
    target_link_libraries(show curses)
endif()

add_custom_target(t COMMAND ./show DEPENDS show)
add_custom_target(e COMMAND vi ../show.cc)
add_custom_target(gdb COMMAND gdb ./show)
