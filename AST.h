#pragma once

#include <memory>
#include "AST.h"
#include "Type.h"

class ExprAST {
public:
	virtual ~ExprAST() {}
public:
	virtual void print() = 0;
};

class NumExprAST : public ExprAST {
public:
	NumExprAST(double v) : value(v) {}
public:
	void print() { cout << "NumExpr: " << value << newline; }
private:
	double value;
};

class ArgumentAST {
public:
	ArgumentAST(Type type, std::string name) : type(type), name(name) {}
public:
	void addValue(double v) { value = v; }
    Type getType() { return type; }
private:
	Type type;
	std::string name;
	double value;
};

class ShaderPrototypeAST {
public:
	ShaderPrototypeAST(std::string shaderName, std::unique_ptr<std::vector<std::unique_ptr<ArgumentAST>>> arguments) : name(shaderName), arguments(std::move(arguments)) {}
private:
	std::string name;
	std::unique_ptr<std::vector<std::unique_ptr<ArgumentAST>>> arguments;
};

class SurfaceShaderAST {
public:
	SurfaceShaderAST(std::unique_ptr<ShaderPrototypeAST> p) : prototype(std::move(p)) {}
private:
	std::unique_ptr<ShaderPrototypeAST> prototype;
};
