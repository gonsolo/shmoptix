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

		llvm::outs() << *(l->getType()) << " " << *(r->getType()) << newline;

		if (l->getType() == CodeGen.pointerToColorType && r->getType() == CodeGen.pointerToColorType) {
            llvm::outs() << "WORKS? color*/color*" << newline;
			auto alloc = Builder.CreateAlloca(CodeGen.pointerToColorType);
			Builder.CreateStore(r, alloc);
            auto load = Builder.CreateLoad(alloc);

            auto zero = Builder.getInt32(0);
            auto  one = Builder.getInt32(1);
            auto  two = Builder.getInt32(2);

            std::vector<llvm::Value*> idx{zero, zero};
            auto elem = Builder.CreateInBoundsGEP(load, idx);
            auto loadElem = Builder.CreateLoad(elem);
            std::vector<llvm::Value*> idx2{zero, zero};
            auto storeElem = Builder.CreateInBoundsGEP(l, idx2);
            Builder.CreateStore(loadElem, storeElem);

            idx[1] = one;
            elem = Builder.CreateInBoundsGEP(load, idx);
            loadElem = Builder.CreateLoad(elem);
            idx2[1] = one;
            storeElem = Builder.CreateInBoundsGEP(l, idx2);
            Builder.CreateStore(loadElem, storeElem);

            idx[1] = two;
            elem = Builder.CreateInBoundsGEP(load, idx);
            loadElem = Builder.CreateLoad(elem);
            idx2[1] = two;
            storeElem = Builder.CreateInBoundsGEP(l, idx2);
            ret = Builder.CreateStore(loadElem, storeElem);
        }
		else if (l->getType() == CodeGen.floatType && r->getType() == CodeGen.floatType) {
			llvm::outs() << "TODO: float/float" << newline;
		}
		else if (l->getType() == CodeGen.pointerToFloatType && r->getType() == CodeGen.floatType) {
			auto alloc = Builder.CreateAlloca(CodeGen.floatType);
			Builder.CreateStore(r, alloc);
			auto load = Builder.CreateLoad(alloc);
			ret = Builder.CreateStore(load, l);
			llvm::outs() << "DONE: float*/float" << newline;
		}
		else {
			llvm::outs() << "TODO: unknown" << newline;
		}

		return ret;
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
		llvm::outs() << "FunctionCallAST " << name << space << argument << newline;
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
		llvm::outs() << "BinaryExprAST" << newline;
		lhs->print();
		rhs->print();
	}
	llvm::Value* codegen() {
		//auto l = lhs->codegen();
		//auto r = rhs->codegen();

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
	SurfaceShaderAST(std::unique_ptr<ShaderPrototypeAST> prototype, std::unique_ptr<ExprAST> body) : prototype(std::move(prototype)), body(std::move(body)) {}
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

