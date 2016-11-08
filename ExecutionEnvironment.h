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
				llvm::outs() << "Failed to create engine: " << errorString << newline;
				exit(EXIT_FAILURE);
			}
			engine->addGlobalMapping(leading_underscore + "Ci", (uint64_t)Ci.get());
			//engine->addGlobalMapping(leading_underscore + "Ci", (uint64_t)&Ci);
		}

	public:
		void dump() {
			llvm::outs() << "Ci: " << Ci << newline;
		}

		void runFunction(const std::string& name, llvm::Function* oldfunction) {

			uint64_t address = engine->getFunctionAddress(name);
			void(*function)(float, float[3]);
			//void(*function)(float[3]);
			//void(*function)(float);
			function = reinterpret_cast<decltype(function)>(address);
			float Kd = 1.f;
			float Cs[3]{ 13.f, 26.f, 39.f };
			//float Cs{ 13.f };
			function(Kd, Cs);
		}

	private:
		llvm::ExecutionEngine* engine;
		Color Ci{ 99.f, 66.f, 33.f };
		//float Ci{ 0.f };
	};

}
