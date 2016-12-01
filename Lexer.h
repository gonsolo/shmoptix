#pragma once

#include <fstream>
#include "global.h"

namespace shmoptix {

	enum Token {
		tok_eof = -1,

		// Types
		tok_surface = -2,
		tok_normal = -3,

		// Primary
		tok_identifier = -10,
		tok_number = -11,

		// Operators
		tok_paren_open = -20,
		tok_paren_close = -21,
		tok_brace_open = -22,
		tok_brace_close = -23,
		tok_equals = -24,
		tok_star = -25,
		tok_slash = -26,
		tok_semicolon = -27,
		tok_comma = -28,

	};

std::ostream& operator<<(std::ostream& out, Token token)
{
	switch (token) {
	case tok_eof:			out << "eof";			break;
	case tok_surface:		out << "surface";		break;
	case tok_normal:		out << "normal";		break;
	case tok_identifier:	out << "identifier";	break;
	case tok_number:		out << "number";		break;
	case tok_paren_open:	out << "(";				break;
	case tok_paren_close:	out << ")";				break;
	case tok_brace_open:	out << "{";				break;
	case tok_brace_close:	out << "}";				break;
	case tok_equals:		out << "=";				break;
	case tok_star:			out << "*";				break;
	case tok_semicolon:		out << ";";				break;
	case tok_comma:			out << ",";				break;
	default:				out << "unknown token"; break;
	}
	return out;
}


class Lexer : public ErrorHandler {
public:
	Lexer() {}
public:
    void setInput(std::ifstream* i) {
        input = i;
    }

    int getChar() {
		lastChar = input->get();
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
			if (identifier == "normal")
				return tok_normal;
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

		if (lastChar == '/') {
			getChar();
			if (lastChar == '/') {
				do {
					getChar();
				} while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

				if (lastChar != EOF) {
					return getToken();
				}
			}
			else {
				return tok_slash;
			}
		}

		if (lastChar == '(') {
			getChar();
			return tok_paren_open;
		}
		if (lastChar == ')') {
			getChar();
			return tok_paren_close;
		}
		if (lastChar == '{') {
			getChar();
			return tok_brace_open;
		}
		if (lastChar == '}') {
			getChar();
			return tok_brace_close;
		}
		if (lastChar == '=') {
			getChar();
			return tok_equals;
		}
		if (lastChar == '*') {
			getChar();
			return tok_star;
		}
		if (lastChar == ';') {
			getChar();
			return tok_semicolon;
		}
		if (lastChar == ',') {
			getChar();
			return tok_comma;
		}
		if (lastChar == EOF) {
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
	std::ifstream* input;
	std::string identifier;
	double numVal;
};

}
