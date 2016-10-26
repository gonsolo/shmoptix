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

llvm::IRBuilder<> Builder(Context);
llvm::IRBuilder<>& getBuilder() {
	return Builder;
}

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

	Lexer lexer;
	Parser parser(lexer);

	auto shader = parser.parse(matte);
	auto function = shader->codegen();

	cout << "Verifying" << newline;
	if (llvm::verifyModule(*module)) {
		cout << "Error verifying module" << newline;
		exit(EXIT_FAILURE);
	}

	ExecutionEnvironment executionEnvironment(std::move(module));

	executionEnvironment.dump();
	cout << "runFunction" << newline;
	executionEnvironment.runFunction(function);
	executionEnvironment.dump();
	cout << "Done" << endl;
}
