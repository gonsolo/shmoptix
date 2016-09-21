#pragma once

#include <vector>
#include "global.h"
#include "AST.h"
#include "ErrorHandler.h"
#include "Lexer.h"
#include "Type.h"

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
			return Float;
		}
		else if (type == "color") {
			return Color;
		}
		else {
			error("Unknown type");
			return Error;
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

	void parseIdentifier() {
		lexer.getIdentifier();
		getNextToken();
	}

	void parsePrimary() {
		parseIdentifier();
	}

	void parseRHS() {
		expect(tok_star);
		getNextToken();
		parseExpression();
	}

	void parseFunctionCall() {
		expect(tok_paren_open);
		getNextToken();
		parsePrimary();
		expect(tok_paren_close);
		getNextToken();
	}

	void parseBinaryExpression() {
		parsePrimary();
		switch(token) {
		case tok_star:
			parseRHS();
			break;
		case tok_paren_open:
			parseFunctionCall();
			break;
		default:
			cout << token << ' ' << lexer.getIdentifier() << newline;
			error("Error in parseBinaryExpression");
		}
	}

	void parseExpression() {
		parseBinaryExpression();
	}

	void parseAssignmentExpression() {

		expect(tok_identifier, "Error");
		getNextToken();
		expect(tok_equals, "Error");
		getNextToken();
		parseExpression();
		expect(tok_semicolon);
		getNextToken();
	}

	void parseStatement() {
		parseAssignmentExpression();

	}

	void parseStatements() {
		parseStatement();
	}

	void parseShaderBody() {

		expect(tok_brace_open, "Expect open brace");
		getNextToken();
		if(token != tok_brace_close) {
			parseStatements();
		}
		expect(tok_brace_close);
		getNextToken();
	}

	auto parseSurfaceShader() {
		getNextToken();
		auto prototype = parseShaderPrototype();
		parseShaderBody();

		auto surfaceShader = std::make_unique<SurfaceShaderAST>(std::move(prototype));
		return surfaceShader;
	}

	void parse() {
		token = getNextToken();
		switch(token) {
		case tok_eof:
			return;
		case tok_surface:
			surfaceShader = parseSurfaceShader();
			break;
		default:
			error("Parse error");
		}
	}

private:
	Lexer& lexer;
	Token token;
	std::unique_ptr<SurfaceShaderAST> surfaceShader;
};

