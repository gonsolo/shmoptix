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

			//engine->addGlobalMapping(leading_underscore + "Cs", (uint64_t)Cs.get());
			engine->addGlobalMapping(leading_underscore + "Ci", (uint64_t)Ci.get());
		}

	public:

		void dump() {
			cout << "Ci: " << Ci << newline;
		}

		void bla() {}
		void runFunction(const std::string& name, llvm::Function* oldfunction) {

			uint64_t address = engine->getFunctionAddress(name);
			void(*function)(float[3]);
			function = reinterpret_cast<decltype(function)>(address);
			float Cs[3]{ 1.f, 1.f, 1.f };
			function(Cs);
#if 0
			std::vector<llvm::GenericValue> args(1);
			args[0].AggregateVal.resize(3);
			args[0].AggregateVal[0].FloatVal = 1.f;
			args[0].AggregateVal[1].FloatVal = 1.f;
			args[0].AggregateVal[2].FloatVal = 1.f;

			engine->runFunction(function, args);
#endif
		}

	private:

		llvm::ExecutionEngine* engine;

		//Color Cs{1.f};
		Color Ci{ 0.f };
	};

}

