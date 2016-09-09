#pragma once

#include <memory>

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

class ShaderPrototypeAST {
public:
	ShaderPrototypeAST(std::string shaderName) : name(shaderName) {}
private:
	std::string name;
};

class SurfaceShader {
public:
	SurfaceShader(std::unique_ptr<ShaderPrototypeAST> p) : prototype(std::move(p)) {}
private:
	std::unique_ptr<ShaderPrototypeAST> prototype;
};