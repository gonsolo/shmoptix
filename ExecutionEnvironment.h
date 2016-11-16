#pragma once

#include <functional>

#include "llvm/ExecutionEngine/ExecutionEngine.h"

#include "CodeGen.h"
#include "Color.h"
#include "global.h"

namespace shmoptix {

	Vector4 L{ 1.f, 0.f, 0.f }; // dummy light vector

	float dot(Vector4 a, Vector4 b) {
		//return a.value[0] * b.value[0] + a.value[1] * b.value[1] + a.value[2] * b.value[2];
		return 0.f;
	}

	float length(Vector4 vector) {
		return sqrtf(dot(vector, vector));
	}
	Vector4 normalize(Vector4 vector) {
		return vector / length(vector);
	}

	Color Cl{ 1.f }; // light color

	//Color diffuse(Vector3 N) {
	Color diffuse(float N[3]) {
		//Color C{ N };
		llvm::outs() << "N: " << N[0] << space << N[1] << space << N[2] << newline;
		llvm::outs().flush();

		Color C{444.f};

		// Just one light instead of illuminance loop
		//C += Cl * dot(normalize(L), N);
		return C;
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
			//engine->addGlobalMapping(leading_underscore + "N", (uint64_t)N.get());
			engine->addGlobalMapping(leading_underscore + "N", (uint64_t)&N);
			engine->addGlobalMapping(leading_underscore + "diffuse", (uint64_t)diffuse);
		}

	public:
		void dump() {
			llvm::outs() << "Ci: " << Ci << newline;
		}

		void runFunction(const std::string& name, llvm::Function* oldfunction) {

			uint64_t address = engine->getFunctionAddress(name);

			void(*function)(float, float[4]);

			function = reinterpret_cast<decltype(function)>(address);
			float Kd = 3.f;
			alignas(16) float Cs[4]{ 23.f, 26.f, 29.f, 32.f };

			function(Kd, Cs);
		}

	private:
		llvm::ExecutionEngine* engine;
		Color Ci{ 99.f, 66.f, 33.f };
		Vector4 N{ 7.f, 77.f, 777.f };
	};
}
