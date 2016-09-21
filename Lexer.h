#pragma once

#include <fstream>
#include "global.h"

enum Token {
	tok_eof = -1,

	// commands
	tok_surface = -2,

	// primary
	tok_identifier = -10,
	tok_number = -11,

	// operators
	tok_paren_open = -20,
	tok_paren_close = -21,
	tok_brace_open = -22,
	tok_brace_close = -23,
	tok_equals = -24,
	tok_star = -25,
	tok_semicolon = -26,
	tok_comma = -27,
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
		while (isspace(lastChar))
			getChar();

		if (isalpha(lastChar)) {
			identifier = lastChar;
			while (isalnum((getChar())))
				identifier += lastChar;
			if (identifier == "surface")
				return tok_surface;
			return tok_identifier;
		}
		if (isdigit(lastChar)) {
			std::string numString;
			do {
				numString += lastChar;
				getChar();
			} while (isdigit(lastChar));
			numVal = strtod(numString.c_str(), 0);
			return tok_number;
		}

		if (lastChar == '(') {
			getChar();
			return tok_paren_open;
		} if (lastChar == ')') {
			getChar();
			return tok_paren_close;
		} if (lastChar == '{') {
			getChar();
			return tok_brace_open;
		} if (lastChar == '}') {
			getChar();
			return tok_brace_close;
		} if (lastChar == '=') {
			getChar();
			return tok_equals;
		} if (lastChar == '*') {
			getChar();
			return tok_star;
		} if (lastChar == ';') {
			getChar();
			return tok_semicolon;
		} if (lastChar == ',') {
			getChar();
			return tok_comma;
		} if (lastChar == EOF) {
			getChar();
			return tok_eof;
		}

		error("Unknown token");
		return tok_eof;
	}

	double getNumber() { return numVal; }

	std::string getIdentifier() { return identifier; }

	void printToken(Token token) {
		switch (token) {
		case tok_eof:			cout << "eof"; break;
		case tok_surface:		cout << "surface"; break;
		case tok_identifier:	cout << "identifier: " << identifier; break;
		case tok_number:		cout << "number: " << numVal; break;
		case tok_paren_open:	cout << "("; break;
		case tok_paren_close:	cout << ")"; break;
		case tok_brace_open:	cout << "{"; break;
		case tok_brace_close:	cout << "}"; break;
		case tok_equals:		cout << "="; break;
		case tok_star:			cout << "*"; break;
		case tok_semicolon:		cout << ";"; break;
		case tok_comma:			cout << ","; break;
		default:				cout << "unknown token"; break;
		}
		cout << newline;
	}


private:
	int lastChar = ' ';
	std::ifstream& input;
	std::string identifier;
	double numVal;
};
