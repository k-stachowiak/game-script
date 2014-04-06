#include <cstdio>
#include <cassert>
#include <cmath>

#include <string>
#include <functional>

#include "API/MoonEngine.h"
#include "API/Exceptions.h"

/*
 * TODO:
 * - Prepare interpreter error handling infrastructure.
 *     - Runtime exception
 */

bool IsClose(double x, double y) {
	return std::abs(x - y) < 0.01;
}

std::vector<std::function<void()>> tests{

	[]() {
		std::printf("Vector construction failure... ");

		std::string source =
			"(bind arr [\"one\" 2 3.0])";

		moon::CEngine engine;

		try {
			engine.LoadUnitString("test", source);
			const auto& array = engine.GetValue("test", "arr");
			assert(false);
		}
		catch (const moon::ExInterpretationError&) {
		}

		std::printf("OK\n");
	},

	[]() {
		std::printf("Constructing vector... ");

		std::string source =
			"(bind arr [\"one\" \"two\" \"three\"])";

		moon::CEngine engine;
		engine.LoadUnitString("test", source);

		auto arr = engine.GetValue("test", "arr");

		assert(arr.GetType() == moon::EValueType::COMPOUND);
		assert(arr.GetCompoundType() == moon::ECompoundType::ARRAY);

		const auto& compound = arr.GetCompound();

		assert(compound.size() == 3);
		assert(compound[0].GetType() == moon::EValueType::STRING);
		assert(compound[0].GetString() == "one");
		assert(compound[1].GetType() == moon::EValueType::STRING);
		assert(compound[1].GetString() == "two");
		assert(compound[2].GetType() == moon::EValueType::STRING);
		assert(compound[2].GetString() == "three");

		std::printf("OK\n");
	},

	[]() {
		std::printf("Simple function call... ");

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

		std::printf("OK\n");
	},

	[]() {
		std::printf("Simple value retrieval... ");

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

		std::printf("OK\n");
	}
};

int main()
{
	for (auto test : tests) {
		test();
	}

	return 0;
}
