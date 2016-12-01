#pragma once

#include <map>
#include <memory>
#include <string>

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/DerivedTypes.h"
#include "global.h"
#include "AST.h"
#include "Type.h"
#include "CodeGen.h"

#include <stdio.h>

namespace shmoptix {

class AST : public ErrorHandler {
public:
	AST() : Builder(getBuilder()) {}
	virtual ~AST() {}
public:
	virtual void print() = 0;
	virtual llvm::Value* codegen() = 0;
protected:
	llvm::IRBuilder<>& Builder;
};

class ExprAST : public AST {
public:
	virtual ~ExprAST() {}
};

class VariableExprAST : public ExprAST {
public:
	VariableExprAST(const std::string name) : name(name) {}
public:
	void print() {
		llvm::outs() << "VariableExprAST " << name << newline;
	}
	llvm::Value* codegen() { 
		auto value = CodeGen.lookupNamedValue(name);
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
		llvm::outs() << "AssignmentExpressionAST" << newline;
		lhs->print();
		rhs->print();
	}
	llvm::Value* codegen() {
		auto l = lhs->codegen();
		assert(l != nullptr && "Value codegen: lhs returned nullptr!");
		auto r = rhs->codegen();
		assert(r != nullptr && "Value codegen: rhs returned nullptr!");

		llvm::Value* ret = nullptr;

		if (l->getType() == CodeGen.pointerToColorType && r->getType() == CodeGen.pointerToColorType) {
			auto load = Builder.CreateAlignedLoad(r, 16);
			//Builder.CreateStore(load, l);
			Builder.CreateAlignedStore(load, l, 16);
		}
		else if (l->getType() == CodeGen.floatType && r->getType() == CodeGen.floatType) {
			llvm::outs() << "TODO assign float/float" << newline;
		}
		else if (l->getType() == CodeGen.pointerToFloatType && r->getType() == CodeGen.floatType) {
			auto alloc = Builder.CreateAlloca(CodeGen.floatType);
			Builder.CreateStore(r, alloc);
			auto load = Builder.CreateLoad(alloc);
			ret = Builder.CreateStore(load, l);
		}
		else if (l->getType() == CodeGen.pointerToColorType && r->getType() == CodeGen.colorType) {
			Builder.CreateStore(r, l);
		}
		else if (l->getType() == CodeGen.pointerToColorType && r->getType() == CodeGen.floatType) {

			// promote float to color
			auto undef = llvm::UndefValue::get(CodeGen.colorType);
			uint64_t idx = 0;
			auto insert = Builder.CreateInsertElement(undef, r, idx);
			auto zeroInt = llvm::Constant::getNullValue(CodeGen.int4Type);
			auto shuffle = Builder.CreateShuffleVector(insert, undef, zeroInt);

			ret = Builder.CreateStore(shuffle, l);
	}
		else {
			llvm::outs() << "assign: unknown" << newline;
			llvm::outs().flush();
			l->getType()->dump();
			r->getType()->dump();
			llvm::outs().flush();
		}
		llvm::outs().flush();

		return ret;
	}

private:
	std::unique_ptr<ExprAST> lhs;
	std::unique_ptr<ExprAST> rhs;
};


class FunctionCallAST : public ExprAST {
public:
	//FunctionCallAST(const std::string& name, const std::string argument) : name(name), argument(argument) {}
	FunctionCallAST(const std::string& name, const std::string& argument) : name(name), argument(argument) {}
public:
	void print() {
		//llvm::outs() << "FunctionCallAST " << name << space << argument << newline;
		llvm::outs() << "FunctionCallAST " << name << space << argument << newline;
	}
	llvm::Value* codegen() {
		
		std::vector<llvm::Value*> args;
		auto llvmCall = CodeGen.lookupNamedValue(name);
		auto arg = CodeGen.lookupNamedValue(argument);
		args.push_back(arg);
		auto call = Builder.CreateCall(llvmCall, args);
		return call;
#if 0

		auto zero = Builder.getInt32(0);
		std::vector<llvm::Value*> idx0{ zero };
		auto gep = Builder.CreateInBoundsGEP(arg, idx0);
		auto test1 = Builder.CreateAlloca(CodeGen.pointerToColorType);
		auto test2 = Builder.CreateStore(gep, test1);

		auto argType = arg->getType();
		auto llvmCall = CodeGen.lookupNamedValue(name);
		auto callType = llvmCall->getType();
		auto pointerType = llvm::cast<llvm::PointerType>(callType);
		auto functionType = llvm::cast<llvm::FunctionType>(pointerType->getElementType());
		auto paramType = functionType->getParamType(0);

		args.push_back(arg);
		//args.push_back(gep);
#endif

#if 0
		llvm::outs() << "arg type: " << newline;
		llvm::outs().flush();
		argType->dump();
		llvm::outs() << newline;
		llvm::outs().flush();

		llvm::outs() << "call type: " << newline;
		llvm::outs().flush();
		llvmCall->getType()->dump();
		llvm::outs() << newline;
		llvm::outs().flush();

		llvm::outs() << "param type: " << newline;
		llvm::outs().flush();
		paramType->dump();
		llvm::outs() << newline;
		llvm::outs().flush();
#endif
#if 0
		if (argType != paramType) {
			error("Types do not match for function call \"" + name + "\"");
		}

		auto call = Builder.CreateCall(llvmCall, args);

		return call;
#endif
	}
private:
	std::string name;
	std::string argument;
};

class BinaryExprAST : public ExprAST {
public:
	BinaryExprAST(std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) : lhs(std::move(lhs)), rhs(std::move(rhs)) {}
public:
	void print() {
		llvm::outs() << "BinaryExprAST" << newline;
		lhs->print();
		rhs->print();
	}
	llvm::Value* codegen() {
		auto l = lhs->codegen();
		auto r = rhs->codegen();

		if (l->getType() == CodeGen.floatType && r->getType() == CodeGen.pointerToColorType) {

			// promote float to color
			auto undef = llvm::UndefValue::get(CodeGen.colorType);
			uint64_t idx = 0;
			auto insert = Builder.CreateInsertElement(undef, l, idx);
			auto zeroVec = llvm::Constant::getNullValue(CodeGen.int4Type);
			auto shuffle = Builder.CreateShuffleVector(insert, undef, zeroVec);
	
			auto load = Builder.CreateLoad(r);
			auto mul = Builder.CreateBinOp(llvm::Instruction::BinaryOps::FMul, shuffle, load);
			return mul;
		}
		else if (l->getType() == CodeGen.pointerToColorType && r->getType() == CodeGen.floatType) {

			// promote float to color
			auto undef = llvm::UndefValue::get(CodeGen.colorType);
			uint64_t idx = 0;
			auto insert = Builder.CreateInsertElement(undef, r, idx);
			auto zeroVec = llvm::Constant::getNullValue(CodeGen.int4Type);
			auto shuffle = Builder.CreateShuffleVector(insert, undef, zeroVec);
	
			auto load = Builder.CreateLoad(l);
			auto mul = Builder.CreateBinOp(llvm::Instruction::BinaryOps::FMul, load, shuffle);
			return mul;
		}
		else if (l->getType() == CodeGen.floatType && r->getType() == CodeGen.colorType) {

			// promote float to color
			auto undef = llvm::UndefValue::get(CodeGen.colorType);
			uint64_t idx = 0;
			auto insert = Builder.CreateInsertElement(undef, l, idx);
			auto zeroVec = llvm::Constant::getNullValue(CodeGen.int4Type);
			auto shuffle = Builder.CreateShuffleVector(insert, undef, zeroVec);
			auto mul = Builder.CreateBinOp(llvm::Instruction::BinaryOps::FMul, shuffle, r);
			return mul;
		}
		else if (l->getType() == CodeGen.pointerToColorType && r->getType() == CodeGen.colorType) {

			auto load = Builder.CreateLoad(l);
			auto mul = Builder.CreateBinOp(llvm::Instruction::BinaryOps::FMul, load, r);
			return mul;
		}
		else {
			l->getType()->dump();
			r->getType()->dump();
			error("Unimplemented binary expression");
			return nullptr;
		}

	}
private:
	// Assume multiplication for now
	std::unique_ptr<ExprAST> lhs;
	std::unique_ptr<ExprAST> rhs;
};

class NumExprAST : public ExprAST {
public:
	NumExprAST(double v) : value(v) {}
public:
	void print() { llvm::outs() << "NumExpr: " << value << newline; }
	llvm::Value* codegen() {
		return llvm::ConstantFP::get(Context, llvm::APFloat(value));
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
	std::string getName() { return name; }
	double getValue() { return value; }

	void print() {
		llvm::outs() << "Argument " << type << space << name << space << value << newline;
	}

	llvm::Argument* codegen() {
		return nullptr;
	}
private:
	Type type;
	std::string name;
	double value;
};

class DeclarationAST : public AST {
public:
	DeclarationAST(const std::string& name, llvm::Value* value) : name(name), value(value) {}
	virtual ~DeclarationAST() {}
public:
	void print() {}
	llvm::Value* codegen() {
		return nullptr;
	}
public:
	std::string name;
	llvm::Value* value;
};

class ShaderPrototypeAST : public ErrorHandler {
public:

	ShaderPrototypeAST(std::string shaderName, std::unique_ptr<std::vector<std::unique_ptr<ArgumentAST>>> arguments) : name(shaderName), arguments(std::move(arguments)) {}

public:

	void print() {
		llvm::outs() << "ShaderPrototypeAST " << name << newline;
		for (const auto& argument : *arguments) {
			argument->print();
		}
	}

	llvm::Function* codegen() {

		std::vector<llvm::Type*> argumentTypes;
		for (auto& argument : *arguments) {
			switch (argument->getType()) {
			case Type::Float:
				argumentTypes.push_back(CodeGen.floatType);
				break;
			case Type::Color:
				argumentTypes.push_back(CodeGen.pointerToColorType);
				break;
			default:
				error("Unknown Type in Argument codegen!");
			}
		}
		auto functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(Context), argumentTypes, false);
		auto function = llvm::cast<llvm::Function>(module->getOrInsertFunction(name, functionType, llvm::AttributeSet()));

		auto argIt = arguments->begin();
		auto llvmIt = function->arg_begin();

		for (int i = 0; i < arguments->size(); ++i) {
			llvmIt->setName((*argIt)->getName());
			++argIt; ++llvmIt;
		}
		return function;
	}

private:

	std::string name;
	std::unique_ptr<std::vector<std::unique_ptr<ArgumentAST>>> arguments;
};

class SurfaceShaderAST : public AST {
public:
	SurfaceShaderAST(std::unique_ptr<ShaderPrototypeAST> prototype, std::unique_ptr<AST> body) : prototype(std::move(prototype)), body(std::move(body)) {}
public:

	void print() {
		llvm::outs() << "SurfaceShaderAST" << newline;
		prototype->print();
		body->print();
	}

	llvm::Function* codegen() {

		llvm::Function* function = prototype->codegen();
		llvm::BasicBlock* BB = llvm::BasicBlock::Create(Context, "entry", function);
		Builder.SetInsertPoint(BB);
		for (auto& argument : function->args()) {
			CodeGen.insertNameValue(argument.getName(), &argument);
		}
		if (body) {
			body->codegen();
		}
		Builder.CreateRetVoid();
		llvm::verifyFunction(*function);

		return function;
	}
private:
	std::unique_ptr<ShaderPrototypeAST> prototype;
	std::unique_ptr<AST> body;
};

}

