#pragma once 

#include <cstdlib>
#include <iostream>
#include <string>

class ErrorHandler {
protected:
	void error(std::string message) {
		std::cerr << message << std::endl;
		exit(-1);
	}
};

