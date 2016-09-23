#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "ErrorHandler.h"
#include "Lexer.h"
#include "Parser.h"

int main(int argc, char** argv) {

	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " <shader.sl>" << newline;
		exit(EXIT_FAILURE);
	}
	std::string shaderName(argv[1]);
	std::ifstream matte(shaderName);
	if(!matte) {
		std::cerr << "Couldn't open " << shaderName << std::endl;
		exit(EXIT_FAILURE);
	}
	cout << "Parsing" << newline;
	Lexer lexer(matte);
	Parser parser(lexer);
	parser.parse();
	cout << "Done" << newline;
}

