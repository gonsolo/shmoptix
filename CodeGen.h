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


class LLVMCodeGen {
private:
	LLVMCodeGen() : LLVMContext(), Builder(LLVMContext) {
		module = std::make_unique<llvm::Module>("shmoptix module", LLVMContext);
		installGlobalVariables();
	}
	~LLVMCodeGen() {}
public:
	static LLVMCodeGen& get() {
		static LLVMCodeGen instance;
		return instance;
	}
	void installGlobalVariables() {
		llvm::ArrayType* colorType = llvm::TypeBuilder<llvm::types::ieee_float[3], true>::get(LLVMContext);

		llvm::Constant* zero = llvm::ConstantFP::get(LLVMContext, llvm::APFloat(0.f));
		llvm::Constant* zeroColor[3]{ zero, zero, zero };
		llvm::Constant* zeroColorInit = llvm::ConstantArray::get(colorType, zeroColor);

		namedValues["Ci"] = new llvm::GlobalVariable(*module, colorType, false, llvm::GlobalValue::ExternalLinkage, zeroColorInit, "Ci");
		namedValues["Cs"] = new llvm::GlobalVariable(*module, colorType, false, llvm::GlobalValue::ExternalLinkage, zeroColorInit, "Cs");
	}
	void insertNameValue(const std::string& name, llvm::Value* value) {
		namedValues[name] = value;
	}
	llvm::Value* lookupNamedValue(const std::string& name) {
		return namedValues[name];
	}
public:
	llvm::LLVMContext LLVMContext;
	llvm::IRBuilder<> Builder;
	std::unique_ptr<llvm::Module> module;
	std::map<std::string, llvm::Value*> namedValues;
};

