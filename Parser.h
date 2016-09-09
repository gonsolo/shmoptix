#pragma once

#include "global.h"
#include "AST.h"
#include "ErrorHandler.h"
#include "Lexer.h"

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

	void parseArgument() {
		expect(tok_identifier, "Expected argument type");
		std::string type = lexer.getIdentifier();
		getNextToken();
		std::string name = lexer.getIdentifier();
		getNextToken();
		if(token == tok_equals) {
			getNextToken();
			expect(tok_number, "Expected a number");
			double value = lexer.getNumber();
			getNextToken();	
		}
	}

	void parseArguments() {
		while(token != tok_paren_close) {
			parseArgument();
			if(token == tok_comma) {
				getNextToken();
			} else if(token == tok_paren_close) {
				break;
			} else {
				error("parseArguments error");
			}
		}
	}

	std::unique_ptr<ShaderPrototypeAST> parseShaderPrototype() {
		expect(tok_identifier, "Expected shader name!");
		std::string shaderName = lexer.getIdentifier();
		getNextToken();
		expect(tok_paren_open, "Expected '('");
		getNextToken();
		parseArguments();
		getNextToken();

		auto prototype = std::make_unique<ShaderPrototypeAST>(shaderName);
		getNextToken();
		return std::move(prototype);
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

		auto surfaceShader = std::make_unique<SurfaceShader>(std::move(prototype));
		return std::move(surfaceShader);
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
	std::unique_ptr<SurfaceShader> surfaceShader;
};

