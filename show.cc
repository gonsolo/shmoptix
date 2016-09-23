#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/TargetSelect.h"

#include "CodeGen.h"
#include "ErrorHandler.h"
#include "Lexer.h"
#include "Parser.h"

float Cs[3]{ 1, 0, 0 };
float Ci[3]{ 0, 0, 0 };

static LLVMCodeGen& CodeGen = LLVMCodeGen::get();

int main(int argc, char** argv) {

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
	function->dump();

	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();

	//llvm::ExecutionEngine* engine = llvm::EngineBuilder(std::move())
	cout << "Done" << endl;
}
