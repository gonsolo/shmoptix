#pragma once

#include <map>
#include <memory>
#include <string>

#define __STDC_LIMIT_MACROS     // bogus error in XCode
#define __STDC_CONSTANT_MACROS  // ditto

#include "llvm/ADT/APFloat.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"

#include "AST.h"
#include "Type.h"



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
		namedValues["Ci"] = new llvm::GlobalVariable(colorType, false, llvm::GlobalValue::InternalLinkage, nullptr, "Ci");
		namedValues["Cs"] = new llvm::GlobalVariable(colorType, false, llvm::GlobalValue::InternalLinkage, nullptr, "Cs");
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
	//std::map<std::string, llvm::AllocaInst*> namedValues;
};

static LLVMCodeGen& CodeGen = LLVMCodeGen::get();

class ExprAST : public ErrorHandler {
public:
	virtual ~ExprAST() {}
public:
	virtual void print() = 0;
	virtual llvm::Value* codegen() = 0;
};

class VariableExprAST : public ExprAST {
public:
	VariableExprAST(const std::string name) : name(name) {}
public:
	void print() {
		cout << "VariableExprAST " << name << newline;
	}
	llvm::Value* codegen() { 
		llvm::Value* value = CodeGen.lookupNamedValue(name);
		if (!value) {
			error("Unknown variable: " + name);
		}
		return value;
	}
private:
	std::string name;
};

class AssignmentExprAST : public ExprAST {
public:
	AssignmentExprAST(std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
public:
	void print() {
		cout << "AssignmentExpressionAST" << newline;
		lhs->print();
		rhs->print();
	}
	llvm::Value* codegen() {
		llvm::Value* l = lhs->codegen();
		llvm::Value* r = rhs->codegen();
		llvm::LoadInst* load = CodeGen.Builder.CreateLoad(r);
		llvm::StoreInst* store = CodeGen.Builder.CreateStore(load, l);
		return store;

	}
private:
	std::unique_ptr<ExprAST> lhs;
	std::unique_ptr<ExprAST> rhs;
};

class FunctionCallAST : public ExprAST {
public:
	FunctionCallAST(const std::string& name, const std::string argument) : name(name), argument(argument) {}
public:
	void print() {
		cout << "FunctionCallAST " << name << space << argument << newline;
	}
	llvm::Value* codegen() { return nullptr; }
private:
	std::string name;
	std::string argument;
};

class BinaryExprAST : public ExprAST {
public:
	BinaryExprAST(std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
public:
	void print() {
		cout << "BinaryExprAST" << newline;
		lhs->print();
		rhs->print();
	}
	llvm::Value* codegen() { return nullptr; }
private:
	std::unique_ptr<ExprAST> lhs;
	std::unique_ptr<ExprAST> rhs;
};

class NumExprAST : public ExprAST {
public:
	NumExprAST(double v) : value(v) {}
public:
	void print() { cout << "NumExpr: " << value << newline; }
	llvm::Value* codegen() {
		return llvm::ConstantFP::get(CodeGen.LLVMContext, llvm::APFloat(value));
	}
private:
	double value;
};

class ArgumentAST : public ExprAST {
public:
	ArgumentAST(Type type, std::string name) : type(type), name(name) {}
public:
	void addValue(double v) { value = v; }
    Type getType() { return type; }
	void print() {
		cout << "Argument " << type << space << name << space << value << newline;
	}
	llvm::Value* codegen() {
		llvm::Value* value = nullptr;
		return value;
	}
private:
	Type type;
	std::string name;
	double value;
};

class ShaderPrototypeAST {
public:
	ShaderPrototypeAST(std::string shaderName, std::unique_ptr<std::vector<std::unique_ptr<ArgumentAST>>> arguments) : name(shaderName), arguments(std::move(arguments)) {}
public:
	void print() {
		cout << "ShaderPrototypeAST " << name << newline;
		for (const auto& argument : *arguments) {
			argument->print();
		}
	}
	llvm::Function* codegen() {

		llvm::FunctionType* functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(CodeGen.LLVMContext), false);
		llvm::Function* function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, "bla", CodeGen.module.get());
		return function;
	}
private:
	std::string name;
	std::unique_ptr<std::vector<std::unique_ptr<ArgumentAST>>> arguments;
};

class SurfaceShaderAST {
public:
	SurfaceShaderAST(std::unique_ptr<ShaderPrototypeAST> prototype, std::unique_ptr<ExprAST> body) : prototype(std::move(prototype)), body(std::move(body)) {}
public:
	void print() {
		cout << "SurfaceShaderAST" << newline;
		prototype->print();
		body->print();
	}
	void codegen() {

		llvm::Function* function = prototype->codegen();
		llvm::BasicBlock* BB = llvm::BasicBlock::Create(CodeGen.LLVMContext, "entry", function);
		CodeGen.Builder.SetInsertPoint(BB);
		if (body) {
			llvm::Value* retVal = body->codegen();
			CodeGen.Builder.CreateRet(retVal);
			llvm::verifyFunction(*function);
		}
	}
	void dump() {
		CodeGen.module->dump();
	}
private:
	std::unique_ptr<ShaderPrototypeAST> prototype;
	std::unique_ptr<ExprAST> body;
};
