#pragma once

#include <fstream>

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
		return tok_eof;
	}

	double getNumber() { return numVal; }

	std::string getIdentifier() { return identifier; }

private:
	int lastChar = ' ';
	std::ifstream& input;
	std::string identifier;
	double numVal;
};
