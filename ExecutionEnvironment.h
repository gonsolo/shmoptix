#pragma once

#include "llvm/ExecutionEngine/ExecutionEngine.h"

#include "global.h"

class ExecutionEnvironment {
public:
	ExecutionEnvironment(llvm::ExecutionEngine* e) : engine(e) {

		engine->addGlobalMapping("Cs", (uint64_t)&Cs);
		engine->addGlobalMapping("Ci", (uint64_t)&Ci);
	}
public:
	void dump() {
		cout << "Cs: " << Cs << space << ", Ci: " << Ci << newline;
	}
private:
	llvm::ExecutionEngine* engine;
public:
	float Cs = 1.f;
	float Ci = 0.f;
};
