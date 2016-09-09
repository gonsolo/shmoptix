#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "ErrorHandler.h"
#include "Lexer.h"
#include "Parser.h"

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

