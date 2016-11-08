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
		namedValues["N"] = new llvm::GlobalVariable(*module, vector3Type, false, llvm::GlobalValue::ExternalLinkage, nullptr, "N");

		std::vector<llvm::Type*> argumentTypes;
		argumentTypes.push_back(pointerToVector3Type);
		auto functionType = llvm::FunctionType::get(pointerToColorType, argumentTypes, false);
		auto function = llvm::Function::Create(functionType, llvm::GlobalValue::ExternalLinkage, "diffuse", module.get());
		namedValues["diffuse"] = function;
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
	llvm::Type* vector3Type = llvm::TypeBuilder<llvm::types::ieee_float[3], true>::get(Context);
	llvm::Type* pointerToVector3Type = llvm::PointerType::getUnqual(vector3Type);

private:
	// Symbol table
	std::map<std::string, llvm::Value*> namedValues;
};

static LLVMCodeGen CodeGen(Context, *module);

}

