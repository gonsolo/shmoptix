#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#define __STDC_LIMIT_MACROS     // bogus error in XCode
#define __STDC_CONSTANT_MACROS  // ditto

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/Support/TargetSelect.h"

#include "CodeGen.h"
#include "ErrorHandler.h"
#include "ExecutionEnvironment.h"
#include "Lexer.h"
#include "Parser.h"

LLVMCodeGen& CodeGen = LLVMCodeGen::get();

int main(int argc, char** argv) {

	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();

	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " <shader.sl>" << newline;
		exit(EXIT_FAILURE);
	}
	std::string shaderName(argv[1]);
	std::ifstream matte(shaderName);
	if(!matte) {
		std::cerr << "Couldn't open " << shaderName << std::endl;
		exit(EXIT_FAILURE);
	}
	cout << "Parsing" << endl;
	Lexer lexer(matte);
	Parser parser(lexer);
	std::unique_ptr<SurfaceShaderAST> shader = parser.parse();
	llvm::Function* function = shader->codegen();
	//function->dump();

	std::string errorString;

	llvm::Module* module = CodeGen.module.get();
	cout << "show.cc: " << module->getModuleIdentifier() << newline;
	module->dump();

	llvm::ExecutionEngine* engine = llvm::EngineBuilder(std::move(CodeGen.module)).setErrorStr(&errorString).create();
	if (!engine) {
		cerr << "Failed to create engine: " << errorString << newline;
		exit(EXIT_FAILURE);
	}

	ExecutionEnvironment executionEnvironment(engine);

	cout << "Verifying" << newline;
	if (llvm::verifyModule(*module)) {
		cout << "Error verifying module" << newline;
		exit(EXIT_FAILURE);
	}

	executionEnvironment.dump();

	cout << "runFunction" << newline;
	std::vector<llvm::GenericValue> args(0);
	engine->runFunction(function, args);

	executionEnvironment.dump();
	cout << "Done" << endl;
}
