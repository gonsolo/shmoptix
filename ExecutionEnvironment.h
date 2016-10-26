#pragma once

#include "llvm/ExecutionEngine/ExecutionEngine.h"

#include "CodeGen.h"
#include "Color.h"
#include "global.h"

namespace shmoptix {


class ExecutionEnvironment {
public:
	ExecutionEnvironment(std::unique_ptr<llvm::Module> module)  {


		std::string errorString;
		engine = llvm::EngineBuilder(std::move(module)).setErrorStr(&errorString).create();
		if (!engine) {
			cerr << "Failed to create engine: " << errorString << newline;
			exit(EXIT_FAILURE);
		}

		engine->addGlobalMapping(leading_underscore + "Cs", (uint64_t)Cs.get());
		engine->addGlobalMapping(leading_underscore + "Ci", (uint64_t)Ci.get());
	}

public:

	void dump() {
		cout << "Cs: " << Cs << space << ", Ci: " << Ci << newline;
	}

	void runFunction(llvm::Function* function) {

	std::vector<llvm::GenericValue> args(0);
	engine->runFunction(function, args);
	}

private:

	llvm::ExecutionEngine* engine;

	Color Cs{1.f};
	Color Ci{0.f};
	//float Cs[3]{ 1.f, 1.f, 1.f };
	//float Ci[3]{ 0.f, 0.f, 0.f };
};

}

