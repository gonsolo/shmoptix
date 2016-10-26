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
		//auto floatType = llvm::TypeBuilder<llvm::types::ieee_float, true>::get(Context);
		auto colorType = llvm::TypeBuilder<llvm::types::ieee_float[3], true>::get(Context);
		namedValues["Cs"] = new llvm::GlobalVariable(*module, colorType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "Cs");
		namedValues["Ci"] = new llvm::GlobalVariable(*module, colorType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "Ci");
	}

	void insertNameValue(const std::string& name, llvm::Value* value) {
		namedValues[name] = value;
	}

	auto lookupNamedValue(const std::string& name) {
		return namedValues[name];
	}

private:
	std::map<std::string, llvm::Value*> namedValues;
};

static LLVMCodeGen CodeGen(Context, *module);

}

