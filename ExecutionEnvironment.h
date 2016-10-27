#pragma once

#include <functional>

#include "llvm/ExecutionEngine/ExecutionEngine.h"

#include "CodeGen.h"
#include "Color.h"
#include "global.h"

namespace shmoptix {

	class ExecutionEnvironment {
	public:
		ExecutionEnvironment(std::unique_ptr<llvm::Module> module) {

			std::string errorString;
			engine = llvm::EngineBuilder(std::move(module)).setErrorStr(&errorString).create();
			if (!engine) {
				cerr << "Failed to create engine: " << errorString << newline;
				exit(EXIT_FAILURE);
			}
			engine->addGlobalMapping(leading_underscore + "Ci", (uint64_t)Ci.get());
		}

	public:
		void dump() {
			cout << "Ci: " << Ci << newline;
		}

		void runFunction(const std::string& name, llvm::Function* oldfunction) {

			uint64_t address = engine->getFunctionAddress(name);
			void(*function)(float[3]);
			function = reinterpret_cast<decltype(function)>(address);
			float Cs[3]{ 1.f, 1.f, 1.f };
			function(Cs);
		}

	private:
		llvm::ExecutionEngine* engine;
		Color Ci{ 0.f };
	};

}

