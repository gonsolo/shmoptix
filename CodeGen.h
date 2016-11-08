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

namespace shmoptix {

class LLVMCodeGen {
public:

	LLVMCodeGen(llvm::LLVMContext& context, llvm::Module& module) {
		installGlobalVariables();
	}

	~LLVMCodeGen() {}

public:

	void installGlobalVariables() {
		namedValues["Ci"] = new llvm::GlobalVariable(*module, colorType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "Ci");
		//namedValues["Ci"] = new llvm::GlobalVariable(*module, floatType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "Ci");
	}

	void insertNameValue(const std::string& name, llvm::Value* value) {
		namedValues[name] = value;
	}

	auto lookupNamedValue(const std::string& name) {
		return namedValues[name];
	}

public:
	// Type cache
	llvm::Type* floatType = llvm::TypeBuilder<llvm::types::ieee_float, true>::get(Context);
	llvm::Type* pointerToFloatType = llvm::PointerType::getUnqual(floatType);
	llvm::Type* colorType = llvm::TypeBuilder<llvm::types::ieee_float[3], true>::get(Context);
	llvm::Type* pointerToColorType = llvm::PointerType::getUnqual(colorType);

private:
	// Symbol table
	std::map<std::string, llvm::Value*> namedValues;
};

static LLVMCodeGen CodeGen(Context, *module);

}

