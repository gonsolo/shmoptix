#pragma once

#include <array>

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"

#include "global.h"

class LLVMCodeGen {
public:

	LLVMCodeGen(llvm::LLVMContext& context, llvm::Module& module) {
		//module = std::make_unique<llvm::Module>("shmoptix module", LLVMContext);
		installGlobalVariables();
	}

	~LLVMCodeGen() {}

public:

	void installGlobalVariables() {
		llvm::ArrayType* colorType = llvm::TypeBuilder<llvm::types::ieee_float[3], true>::get(Context);
		llvm::Type* floatType = llvm::TypeBuilder<llvm::types::ieee_float, true>::get(Context);

		llvm::Constant* zeroFloat = llvm::ConstantFP::get(Context, llvm::APFloat(0.f));
		llvm::Constant* zeroColor[3]{ zeroFloat, zeroFloat, zeroFloat };
		llvm::Constant* zeroColorInit = llvm::ConstantArray::get(colorType, zeroColor);

		//namedValues["Ci"] = new llvm::GlobalVariable(*module, colorType, false, llvm::GlobalValue::ExternalLinkage, zeroColorInit, "Ci");
		//namedValues["Cs"] = new llvm::GlobalVariable(*module, colorType, false, llvm::GlobalValue::ExternalLinkage, zeroColorInit, "Cs");
		namedValues["Cs"] = new llvm::GlobalVariable(*module, floatType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "Cs");
		namedValues["Ci"] = new llvm::GlobalVariable(*module, floatType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "Ci");
	}

	void insertNameValue(const std::string& name, llvm::Value* value) {
		namedValues[name] = value;
	}

	llvm::Value* lookupNamedValue(const std::string& name) {
		return namedValues[name];
	}

public:
	//llvm::LLVMContext& myLLVMContext;
	//llvm::IRBuilder<> Builder;
	std::map<std::string, llvm::Value*> namedValues;
	//llvm::Module& module;
};

static LLVMCodeGen CodeGen(Context, *module);

