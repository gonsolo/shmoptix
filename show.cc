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
	//function->dump();


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

	engine->addGlobalMapping("Cs", (uint64_t)Cs);
	engine->addGlobalMapping("Ci", (uint64_t)Ci);

	//float (*Cs)[3] = static_cast<float(*)[3]>(engine->getPointerToGlobal(llvm::dyn_cast<llvm::GlobalVariable>(CodeGen.lookupNamedValue("Cs"))));
	//(*Cs)[0] = 1.f;
	//(*Cs)[1] = 1.f;
	//(*Cs)[2] = 1.f;
	//float (*Ci)[3] = static_cast<float(*)[3]>(engine->getPointerToGlobal(llvm::dyn_cast<llvm::GlobalVariable>(CodeGen.lookupNamedValue("Ci"))));
	//(*Ci)[0] = 0.f;
	//(*Ci)[1] = 0.f;
	//(*Ci)[2] = 0.f;

	cout << "Verifying" << newline;
	if (llvm::verifyModule(*module)) {
		cout << "Error verifying module" << newline;
		exit(EXIT_FAILURE);
	}

	//float *Cs = static_cast<float*>(engine->getPointerToGlobal(llvm::dyn_cast<llvm::GlobalVariable>(CodeGen.lookupNamedValue("Cs"))));

	//auto test1 = llvm::dyn_cast<llvm::GlobalVariable>(CodeGen.lookupNamedValue("Cs"));
	//auto test2 = engine->getPointerToGlobal(test1);
	//float* test3 = static_cast<float*>(test2);

	llvm::Type* floatType = llvm::TypeBuilder<llvm::types::ieee_float, true>::get(CodeGen.LLVMContext);
	llvm::Constant* zeroFloat = llvm::ConstantFP::get(CodeGen.LLVMContext, llvm::APFloat(0.f));

	auto zeroFloatFP = llvm::dyn_cast<llvm::ConstantFP>(zeroFloat);
	cout << "zeroFloatFP" << newline;
	//zeroFloatFP->dump();

	llvm::GlobalVariable* laber = new llvm::GlobalVariable(*module, floatType, false, llvm::GlobalValue::ExternalLinkage, zeroFloat, "laber");

	//module->getOrInsertGlobal("laber", floatType);
	//llvm::GlobalVariable* laberLookup = module->getNamedGlobal("laber");
	

	//std::vector<llvm::GenericValue> args(0);
	//args[0].IntVal = llvm::APInt(32, 11);

	*Cs = 13.f;
	*Ci = 4.f;
	//cout << "pre Cs now: " << *Cs /*<< space << (*Cs)[1] << space << (*Cs)[2] */<< newline;
	//cout << "pre Ci now: " << *Ci /*<< space << (*Ci)[1] << space << (*Ci)[2] */<< newline;

	cout << "Cs: " << *Cs << space << ", Ci: " << *Ci << newline;

	cout << "runFunction" << newline;
	std::vector<llvm::GenericValue> args(0);
	//llvm::GenericValue result = engine->runFunction(function, args);
	engine->runFunction(function, args);
	//cout << "Result: " <<  result.FloatVal << newline;

	cout << "Cs: " << *Cs << space << ", Ci: " << *Ci << newline;

	//float *Ci = static_cast<float*>(engine->getPointerToGlobal(llvm::dyn_cast<llvm::GlobalVariable>(CodeGen.lookupNamedValue("Ci"))));
	//uint64_t Ci = engine->getGlobalValueAddress("Ci");
	//float bla = *(float*)Ci;
	//cout << "bla: " << bla << newline;

	//cout << "Cs now: " << (*Cs)[0] /*<< space << (*Cs)[1] << space << (*Cs)[2] */<< newline;
	//cout << "Ci now: " << (*Ci)[0] /*<< space << (*Ci)[1] << space << (*Ci)[2] */<< newline;
	//cout << "post Cs now: " << *Cs /*<< space << (*Cs)[1] << space << (*Cs)[2] */<< newline;
	//cout << "post Ci now: " << *Ci /*<< space << (*Ci)[1] << space << (*Ci)[2] */<< newline;

	cout << "Done" << endl;
}
