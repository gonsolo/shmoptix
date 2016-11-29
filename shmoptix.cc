#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS     // bogus error in XCode
#endif
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS  // ditto
#endif

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/Support/TargetSelect.h"

#include "CodeGen.h"
#include "ErrorHandler.h"
#include "ExecutionEnvironment.h"
#include "Lexer.h"
#include "Parser.h"


namespace shmoptix {

llvm::IRBuilder<> Builder(Context);
llvm::IRBuilder<>& getBuilder() {
	return Builder;
}

}

using namespace shmoptix;



int main(int argc, char** argv) {



	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();

	if (argc != 2) {
		llvm::outs() << "Usage: " << argv[0] << " <shader.sl>" << newline;
		exit(EXIT_FAILURE);
	}
	std::string fileName(argv[1]);
	std::ifstream shaderStream(fileName);
	if(!shaderStream) {
		std::cerr << "Couldn't open " << fileName << std::endl;
		exit(EXIT_FAILURE);
	}
	llvm::outs() << "Parsing" << newline;

	Lexer lexer;
	Parser parser(lexer);

	auto shader = parser.parse(shaderStream);
	auto function = shader->codegen();

	module->dump();
	//return 0;

	llvm::outs() << "Verifying" << newline;
	if (llvm::verifyModule(*module, &llvm::dbgs())) {
		llvm::outs() << "Error verifying module" << newline;
		exit(0);
	}
	llvm::outs() << "Verification ok." << newline;

	ExecutionEnvironment executionEnvironment(std::move(module));
	executionEnvironment.dump();
	executionEnvironment.runFunction(function->getName().str());
	executionEnvironment.dump();
	llvm::outs() << "Done" << newline;
}
