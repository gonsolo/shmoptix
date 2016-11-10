#pragma once 

#include <cstdlib>
#include <string>
#include "global.h"

namespace shmoptix {

	class ErrorHandler {
	protected:
		void error(std::string message) {
			llvm::outs() << message << newline;
			exit(EXIT_FAILURE);
		}
	};

}

