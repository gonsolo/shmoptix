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
};

class Lexer {
public:
	Lexer(std::ifstream& shader) : input(shader) {} 
public:
	Token getToken() {
		int lastChar = ' ';
		while(isspace(lastChar))
			lastChar = input.get();
		if(isalpha(lastChar)) {
			identifier = lastChar;
			while(isalnum((lastChar = input.get())))
				identifier += lastChar;
			if(identifier == "surface")
				return tok_surface;
			return tok_identifier;
		}
		if(isdigit(lastChar)) {
			std::string numString;
			do {
				numString += lastChar;
				lastChar = input.get();
			} while (isdigit(lastChar));	
			numVal = strtod(numString.c_str(), 0);
			return tok_number;
		}

		if(lastChar == '(')
			return tok_paren_open;
		if(lastChar == ')')
			return tok_paren_close;
		if(lastChar == '{')
			return tok_brace_open;
		if(lastChar == '}')
			return tok_brace_close;
		if(lastChar == '=')
			return tok_equals;
		if(lastChar == '*')
			return tok_star;
		if(lastChar == ';')
			return tok_semicolon;
		if(lastChar == EOF)
			return tok_eof;

		std::cerr << (char)lastChar << std::endl;
		error("Unknown token");
	}

	double getNumber() { return numVal; }
private:
	void error(std::string message) {
		std::cerr << message << std::endl;
		exit(-1);
	}
private:
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

class Parser {
public:
	Parser(Lexer& l) : lexer(l) {}
public:
	Token getNextToken() {
		return token = lexer.getToken();
	}

	std::unique_ptr<ExprAST> parseNumExpr(double value) {
		auto result = std::make_unique<NumExprAST>(value);
		getNextToken();
		return std::move(result);
	}

	void parse() {
		token = getNextToken();
		while(token != tok_eof) {
			if(token == tok_number) {
				auto numExpr = parseNumExpr(lexer.getNumber());
				numExpr->print();
			} else {
				getNextToken();
			}
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

