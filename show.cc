#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

using std::cout;
const char newline = '\n';

enum Token {
	tok_eof		= -1,

	// commands
	tok_surface	= -2,

	// primary
	tok_identifier	= -10,
	tok_number	= -11,

	// operators
	tok_paren_open	= -20,
	tok_paren_close	= -21,
	tok_brace_open	= -22,
	tok_brace_close	= -23,
	tok_equals	= -24,
	tok_star	= -25,
	tok_semicolon	= -26,
	tok_comma	= -27,
};

class ErrorHandler {
protected:
	void error(std::string message) {
		std::cerr << message << std::endl;
		exit(-1);
	}
};

class Lexer : public ErrorHandler {
public:
	Lexer(std::ifstream& shader) : input(shader) {} 
public:
	int getChar() {
		lastChar = input.get();
		return lastChar;
	}
	Token getToken() {
		while(isspace(lastChar))
			getChar();
	
		if(isalpha(lastChar)) {
			identifier = lastChar;
			while(isalnum((getChar())))
				identifier += lastChar;
			if(identifier == "surface")
				return tok_surface;
			return tok_identifier;
		}
		if(isdigit(lastChar)) {
			std::string numString;
			do {
				numString += lastChar;
				getChar();
			} while (isdigit(lastChar));	
			numVal = strtod(numString.c_str(), 0);
			return tok_number;
		}

		if(lastChar == '(') {
			getChar();
			return tok_paren_open;
		} if(lastChar == ')') {
			getChar();
			return tok_paren_close;
		} if(lastChar == '{') {
			getChar();
			return tok_brace_open;
		} if(lastChar == '}') {
			getChar();
			return tok_brace_close;
		} if(lastChar == '=') {
			getChar();
			return tok_equals;
		} if(lastChar == '*') {
			getChar();
			return tok_star;
		} if(lastChar == ';') {
			getChar();
			return tok_semicolon;
		} if(lastChar == ',') {
			getChar();
			return tok_comma;
		} if(lastChar == EOF) {
			getChar();
			return tok_eof;
		}

		error("Unknown token");
	}

	double getNumber() { return numVal; }

	std::string getIdentifier() { return identifier; }

private:
	int lastChar = ' ';
	std::ifstream& input;
	std::string identifier;
	double numVal;
};

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
	void parseShaderPrototype() {
		expect(tok_identifier, "Expected shader name!");
		std::string shaderName = lexer.getIdentifier();
		getNextToken();
		expect(tok_paren_open, "Expected '('");
		getNextToken();
		parseArguments();
		getNextToken();	
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

	void parseSurfaceShader() {
		getNextToken();
		parseShaderPrototype();
		parseShaderBody();
	}

	void parse() {
		token = getNextToken();
		switch(token) {
		case tok_eof:
			return;
		case tok_surface:
			parseSurfaceShader();
			break;
		default:
			error("Parse error");
		}
	}

private:
	Lexer& lexer;
	Token token;
};

int main() {
	cout << "Reading shader" << newline;
	std::ifstream matte("../matte.sl");
	if(!matte) {
		std::cerr << "Couldn't open matte.sl" << std::endl;
	}
	cout << "Parsing" << newline;
	Lexer lexer(matte);
	Parser parser(lexer);
	parser.parse();

	cout << "Done" << newline;
}

