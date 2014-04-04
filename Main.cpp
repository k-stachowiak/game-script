#include <cstdio>
#include <cassert>
#include <cmath>

#include <string>
#include <functional>

#include "API/MoonEngine.h"

/*
 * TODO:
 * - Unify exceptions.
 *     - Move the derived exceptions to the respective modules
 *     - Only have parsetime and runtime top-level errors
 *     - Make sure, that no logic depends on the exceptions so that they can be switched off.
 * - Add assertions for arrays to be of one type.
 * - Prepare interpreter error handling infrastructure.
 *     - Runtime exception
 */

bool IsClose(double x, double y) {
	return std::abs(x - y) < 0.01;
}

std::vector<std::function<void()>> tests {
	[]() {
		std::printf("Simple function call.\n");

		std::string source =
			"(bind module (func (x y)\n"
			"\t(bind x2 (* x x))\n"
			"\t(bind y2 (* y y))\n"
			"\t(sqrt (+ x2 y2))\n"
			"))";

		moon::CEngine engine;
		engine.LoadUnitString("test", source);

		auto ix = moon::CValue::MakeInteger(1);
		auto iy = moon::CValue::MakeInteger(1);
		auto iResult = engine.CallFunction("test", "module", { ix, iy });
		assert(iResult.GetType() == moon::EValueType::INTEGER);
		assert(iResult.GetInteger() == 1);

		auto rx = moon::CValue::MakeReal(1);
		auto ry = moon::CValue::MakeReal(1);
		auto rResult = engine.CallFunction("test", "module", { rx, ry });
		assert(rResult.GetType() == moon::EValueType::REAL);
		assert(IsClose(rResult.GetReal(), std::sqrt(2)));

		std::printf("OK\n\n");
	},

	[]() {
		std::printf("Simple value retrieval.\n");

		std::string source =
			"# Useless constant\n"
			"(bind neg_thousand (- 0 1000))\n"
			"\n"
			"# Less useless constant\n"
			"(bind pi 3.1415)";

		moon::CEngine engine;
		engine.LoadUnitString("test", source);

		auto neg_thousand = engine.GetValue("test", "neg_thousand");
		auto pi = engine.GetValue("test", "pi");

		assert(neg_thousand.GetType() == moon::EValueType::INTEGER);
		assert(pi.GetType() == moon::EValueType::REAL);

		assert(neg_thousand.GetInteger() == -1000);
		assert(IsClose(pi.GetReal(), 3.1415));

		std::printf("OK\n\n");
	}
};

int main()
{
	for (auto test : tests) {
		test();
	}

	return 0;
}
