#pragma once

#include <vector>
#include "global.h"
#include "AST.h"
#include "ErrorHandler.h"
#include "Lexer.h"
#include "Type.h"

namespace shmoptix {

class Parser : public ErrorHandler {
public:
	Parser(Lexer& l) : lexer(l) {}
public:
	Token getNextToken() {
		return token = lexer.getToken();
	}

	void expect(Token expected, std::string message) {
		if(token != expected) {
			error(message);
		}
	}

	void expect(Token expected) {
		expect(token, "Error");
	}

	std::unique_ptr<ExprAST> parseNumExpr(double value) {
		auto result = std::make_unique<NumExprAST>(value);
		getNextToken();
		return std::move(result);
	}

	Type parseType() {
		expect(tok_identifier, "Expected argument type");
		std::string type = lexer.getIdentifier();
		if (type == "float") {
			return Type::Float;
		}
		else if (type == "color") {
			return Type::Color;
		}
		else {
			error("Unknown type");
			return Type::Error;
		}
	}

	auto parseArgument() {

		Type type = parseType();

		getNextToken();
		std::string name = lexer.getIdentifier();

		auto argument = std::make_unique<ArgumentAST>(type, name);

		getNextToken();
		if(token == tok_equals) {
			getNextToken();
			expect(tok_number, "Expected a number");
			double value = lexer.getNumber();
			argument->addValue(value);
			getNextToken();	
		}

		return argument;
	}

	auto parseArguments() {
		auto arguments = std::make_unique<std::vector<std::unique_ptr<ArgumentAST>>>();

		while(token != tok_paren_close) {
			auto argument = parseArgument();
			arguments->push_back(std::move(argument));
			if(token == tok_comma) {
				getNextToken();
			} else if(token == tok_paren_close) {
				break;
			} else {
				error("parseArguments error");
			}
		}
		return arguments;
	}

	std::unique_ptr<ShaderPrototypeAST> parseShaderPrototype() {

		expect(tok_identifier, "Expected shader name!");
		std::string shaderName = lexer.getIdentifier();
		getNextToken();
		expect(tok_paren_open, "Expected '('");
		getNextToken();
		auto arguments = parseArguments();
		getNextToken();
		auto prototype = std::make_unique<ShaderPrototypeAST>(shaderName, std::move(arguments));
		return prototype;
	}

	std::unique_ptr<ExprAST> parseIdentifier() {
		expect(tok_identifier, "Error identifier");
		auto name = lexer.getIdentifier();
		getNextToken();
		return std::make_unique<VariableExprAST>(name);

	}

	std::unique_ptr<ExprAST> parsePrimaryExpression() {
		return parseIdentifier();
	}

	std::unique_ptr<ExprAST> parseRHS() {
		expect(tok_star);
		getNextToken();
		auto expression = parseExpression();
		return expression;
	}

	std::unique_ptr<ExprAST> parseBinaryExpression(std::unique_ptr<ExprAST> lhs) {
		//auto lhs = parsePrimaryExpression();
		std::unique_ptr<ExprAST> rhs;
		switch(token) {
		case tok_semicolon:
			return lhs;
			break;
		case tok_star:
			rhs = parseRHS();
			break;
		default:
			llvm::outs() << token << ' ' << lexer.getIdentifier() << newline;
			error("Error in parseBinaryExpression");
		}
        std::unique_ptr<ExprAST> binaryExpression = std::make_unique<BinaryExprAST>(std::move(lhs), std::move(rhs));
		return binaryExpression;
	}

	std::unique_ptr<ExprAST> parseFunctionCall(const std::string& name) {
		expect(tok_paren_open);
		getNextToken();

		std::string argument;
		if (token == tok_identifier) {
			argument = lexer.getIdentifier();
			getNextToken();
		}
		expect(tok_paren_close);
		getNextToken();
		//std::unique_ptr<ExprAST> functionCall = std::make_unique<FunctionCallAST>(first, argument);
		std::unique_ptr<ExprAST> functionCall = std::make_unique<FunctionCallAST>(name, argument);
		return functionCall;
	}

	std::unique_ptr<ExprAST> parseExpression() {

		expect(tok_identifier, "Error identifier");
		auto first = lexer.getIdentifier();
		getNextToken();
		if (token == tok_paren_open) {
			return parseFunctionCall(std::move(first));
		}
		else {
			auto lhs = std::make_unique<VariableExprAST>(first);
			return parseBinaryExpression(std::move(lhs));
		}
	}

	std::unique_ptr<ExprAST> parseAssignmentExpression() {

		auto lhs = parsePrimaryExpression();
		expect(tok_equals, "Error equals");
		getNextToken();
		auto rhs = parseExpression();
		std::unique_ptr<ExprAST> assignmentExpression = std::make_unique<AssignmentExprAST>(std::move(lhs), std::move(rhs));
		return assignmentExpression;
	}

	std::unique_ptr<AST> parseDeclaration() {

		expect(tok_normal);
		getNextToken();
		auto name = lexer.getIdentifier();
		auto decl = new llvm::GlobalVariable(*module, CodeGen.normalType, false, llvm::GlobalValue::ExternalLinkage, nullptr, name);
		CodeGen.insertNameValue(name, decl);

		std::unique_ptr<AST> declaration = std::make_unique<DeclarationAST>(name, decl);
		return declaration;
	}


	auto parseStatement() {
		// parse first token
		// parse postfix expression suffix

		std::unique_ptr<AST> statement;

		if (token == tok_normal) {
			statement = parseDeclaration();
		}
		else {
			statement = parseAssignmentExpression();
		}
		expect(tok_semicolon);
		getNextToken();

		return statement;
	}

	auto parseStatements() {
		return parseStatement();
	}

	auto parseShaderBody() {

		expect(tok_brace_open, "Expect open brace");
		getNextToken();
		std::unique_ptr<AST> body;
		if(token != tok_brace_close) {
			body = parseStatements();
		}
		expect(tok_brace_close);
		getNextToken();
		return body;
	}

	auto parseSurfaceShader() {
		getNextToken();
		auto prototype = parseShaderPrototype();
		auto body = parseShaderBody();
		auto surfaceShader = std::make_unique<SurfaceShaderAST>(std::move(prototype), std::move(body));
		return surfaceShader;
	}

    std::unique_ptr<SurfaceShaderAST> parse(std::ifstream& shader) {

        lexer.setInput(&shader);
		token = getNextToken();
		switch(token) {
		case tok_eof:
			return nullptr;
		case tok_surface:
			surfaceShader = parseSurfaceShader();
			break;
		default:
			error("Parse error");
		}
		return std::move(surfaceShader);
	}

private:
	Lexer& lexer;
	Token token;
	std::unique_ptr<SurfaceShaderAST> surfaceShader;
};

}

