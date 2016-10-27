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

namespace shmoptix {

class AST : public ErrorHandler {
public:
	AST() : Builder(getBuilder()){}
	virtual ~AST() {}
protected:
	llvm::IRBuilder<>& Builder;
};

class ExprAST : public AST {
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
		cout << "AssignmentExpressionAST" << newline;
		lhs->print();
		rhs->print();
	}
	llvm::Value* codegen() {
		auto l = lhs->codegen();
		assert(l != nullptr && "Value codegen: lhs returned nullptr!");
		auto r = rhs->codegen();
		assert(r != nullptr && "Value codegen: rhs returned nullptr!");

		cout << "l type: " << std::flush; l->getType()->dump();
		cout << "r type: " << std::flush; r->getType()->dump();

#if 0
		llvm::Value* store = nullptr;

		if (l -> getType() == CodeGen.colorType && r->getType() == CodeGen.floatType) {

			auto load = Builder.CreateLoad(r);
			return store = Builder.CreateStore(load, l);

		} else if (l->getType() == CodeGen.colorType && r->getType() == CodeGen.floatType) {

			// Type convert float -> color

			auto load = Builder.CreateLoad(r);
			auto zero = Builder.getInt32(0);
			auto red = Builder.CreateGEP(l, zero);
			Builder.CreateStore(load, red);
			auto one = Builder.getInt32(1);
			auto green = Builder.CreateGEP(l, one);
			Builder.CreateStore(load, green);
			auto two = Builder.getInt32(2);
			auto blue = Builder.CreateGEP(l, two);
			store = Builder.CreateStore(load, blue);
		}
#endif
		return nullptr;
#if 0
		auto nCs = Builder.CreateAlloca(CodeGen.colorType);
		Builder.CreateStore(r, nCs);
		auto load = Builder.CreateLoad(nCs);
		return Builder.CreateStore(load, l);
#endif
		//auto load = Builder.CreateLoad(r);
		//auto store = Builder.CreateStore(load, l);
		//return Builder.CreateStore(r, l);
		//return nullptr;
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
	llvm::Value* codegen() {
		auto l = lhs->codegen();
		auto r = rhs->codegen();

		return nullptr;
	}
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
		cout << "Argument " << type << space << name << space << value << newline;
	}

	llvm::Argument* codegen() {

		//return nullptr;

		// Make Value
		llvm::Type* llvmType = nullptr;

		switch (type) {
		case Type::Float:
			llvmType = CodeGen.floatType;
			break;
		case Type::Color:
			llvmType = CodeGen.colorType;
			break;
		default:
			error("Unknown Type in Argument codegen!");
		}

		auto argument = new llvm::Argument(llvmType, name);
		CodeGen.insertNameValue(name, argument);
		return argument;
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

		std::vector<llvm::Argument*> llvmArguments;
		for (auto& argument : *arguments) {
			auto arg = argument->codegen();
			llvmArguments.push_back(arg);
		}

		std::vector<llvm::Type*> argumentTypes;
		for (auto& argument : llvmArguments) {
			argumentTypes.push_back(argument->getType());
		}
		auto functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(Context), argumentTypes, false);
		auto function = llvm::cast<llvm::Function>(module->getOrInsertFunction(name, functionType, llvm::AttributeSet()));

		auto args = function->arg_begin();
		for (int i = 0; i < arguments->size(); ++i) {
			args->setName((*arguments)[i]->getName());
			++args;
		}

		return function;
	}

private:

	std::string name;
	std::unique_ptr<std::vector<std::unique_ptr<ArgumentAST>>> arguments;
};

class SurfaceShaderAST : public AST {
public:
	SurfaceShaderAST(std::unique_ptr<ShaderPrototypeAST> prototype, std::unique_ptr<ExprAST> body) : prototype(std::move(prototype)), body(std::move(body)) {}
public:

	void print() {
		cout << "SurfaceShaderAST" << newline;
		prototype->print();
		body->print();
	}

	llvm::Function* codegen() {

		llvm::Function* function = prototype->codegen();
		llvm::BasicBlock* BB = llvm::BasicBlock::Create(Context, "entry", function);
		Builder.SetInsertPoint(BB);
		if (body) {
			body->codegen();
			Builder.CreateRetVoid();
			llvm::verifyFunction(*function);
		}
		return function;
	}
private:
	std::unique_ptr<ShaderPrototypeAST> prototype;
	std::unique_ptr<ExprAST> body;
};

}

