#pragma once

#include <functional>

#include "llvm/ExecutionEngine/ExecutionEngine.h"

#include "CodeGen.h"
#include "Color.h"
#include "global.h"

namespace shmoptix {

	Vector3 dummyLightVector;

	Color diffuse(Vector3 normal) {
		Color color{ 44.f, 55.f, 66.f };

		// TODO
		llvm::outs() << "Calling diffuse" << newline;
		llvm::outs().flush();
		return color;
	}

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
			engine->addGlobalMapping(leading_underscore + "N", (uint64_t)N.get());
			engine->addGlobalMapping(leading_underscore + "diffuse", (uint64_t)diffuse);
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
			float Kd = 2.f;
			float Cs[3]{ 13.f, 26.f, 39.f };
			//float Cs{ 13.f };
			function(Kd, Cs);
		}

	private:
		llvm::ExecutionEngine* engine;
		Color Ci{ 99.f, 66.f, 33.f };
		Vector3 N{ 7.f, 77.f, 777.f };
		//Color diffuse(Vector3 N) {
		//	// TODO
		//}
	};

}
