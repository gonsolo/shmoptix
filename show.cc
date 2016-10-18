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
	function->dump();


	std::string errorString;

	llvm::Module* module = CodeGen.module.get();
	cout << "show.cc: " << module->getModuleIdentifier() << newline;
	module->dump();

	//llvm::LLVMContext Context;
	//std::unique_ptr<llvm::Module> Owner(new llvm::Module("test", Context));
	//llvm::ExecutionEngine* engine = llvm::EngineBuilder(std::move(Owner)).setErrorStr(&errorString).create();

	llvm::ExecutionEngine* engine = llvm::EngineBuilder(std::move(CodeGen.module)).setErrorStr(&errorString).create();
	if (!engine) {
		cerr << "Failed to create engine: " << errorString << newline;
		exit(EXIT_FAILURE);
	}

	//float (*Cs)[3] = static_cast<float(*)[3]>(engine->getPointerToGlobal(llvm::dyn_cast<llvm::GlobalVariable>(CodeGen.lookupNamedValue("Cs"))));
	//(*Cs)[0] = 1.f;
	//(*Cs)[1] = 1.f;
	//(*Cs)[2] = 1.f;
	//float (*Ci)[3] = static_cast<float(*)[3]>(engine->getPointerToGlobal(llvm::dyn_cast<llvm::GlobalVariable>(CodeGen.lookupNamedValue("Ci"))));
	//(*Ci)[0] = 0.f;
	//(*Ci)[1] = 0.f;
	//(*Ci)[2] = 0.f;

	float *Cs = static_cast<float*>(engine->getPointerToGlobal(llvm::dyn_cast<llvm::GlobalVariable>(CodeGen.lookupNamedValue("Cs"))));
	float *Ci = static_cast<float*>(engine->getPointerToGlobal(llvm::dyn_cast<llvm::GlobalVariable>(CodeGen.lookupNamedValue("Cs"))));

	std::vector<llvm::GenericValue> args(0);
	*Cs = 13.f;
	engine->runFunction(function, args);

	//cout << "Cs now: " << (*Cs)[0] /*<< space << (*Cs)[1] << space << (*Cs)[2] */<< newline;
	//cout << "Ci now: " << (*Ci)[0] /*<< space << (*Ci)[1] << space << (*Ci)[2] */<< newline;
	cout << "Cs now: " << *Cs /*<< space << (*Cs)[1] << space << (*Cs)[2] */<< newline;
	cout << "Ci now: " << *Ci /*<< space << (*Ci)[1] << space << (*Ci)[2] */<< newline;

	cout << "Done" << endl;
}
