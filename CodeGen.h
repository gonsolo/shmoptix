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
		alignas(16) llvm::Value* Ci = new llvm::GlobalVariable(*module, colorType, false, llvm::GlobalValue::ExternalLinkage, nullptr, "Ci");
		namedValues["Ci"] = Ci;
		namedValues["N"] = new llvm::GlobalVariable(*module, vector4Type, false, llvm::GlobalValue::ExternalLinkage, nullptr, "N");

		std::vector<llvm::Type*> diffuseArgumentTypes;
		diffuseArgumentTypes.push_back(pointerToVector4Type);
		//auto diffuseType = llvm::FunctionType::get(floatType, diffuseArgumentTypes, false);
		auto diffuseType = llvm::FunctionType::get(colorType, diffuseArgumentTypes, false);
		auto diffuse = llvm::Function::Create(diffuseType, llvm::GlobalValue::ExternalLinkage, "diffuse", module.get());
		namedValues["diffuse"] = diffuse;
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
	llvm::Type* colorType = llvm::VectorType::get(floatType, 4);
	llvm::Type* pointerToColorType = llvm::PointerType::getUnqual(colorType);
	llvm::Type* vector4Type = llvm::VectorType::get(floatType, 4);
	llvm::Type* pointerToVector4Type = llvm::PointerType::getUnqual(vector4Type);
	llvm::Type* normalType = llvm::VectorType::get(floatType, 4);
	llvm::Type* intType = llvm::TypeBuilder<llvm::types::i<32>, true>::get(Context);
	llvm::Type* int4Type = llvm::VectorType::get(intType, 4);
	llvm::Type* voidType = llvm::Type::getVoidTy(Context);

private:
	// Symbol table
	std::map<std::string, llvm::Value*> namedValues;
};

static LLVMCodeGen CodeGen(Context, *module);

}

