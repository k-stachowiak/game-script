#include <cstdio>
#include <cassert>
#include <string>

#include "API/MoonEngine.h"

void SimpleTest()
{
	// Prepare source.
	std::string source =
		"(bind module (func (x y)\n"
		"\t(bind x2 (* x x))\n"
		"\t(bind y2 (* y y))\n"
		"\t(sqrt (+ x2 y2))\n"
		"))";

	// Prepare engine.
	moon::CEngine engine;
	engine.LoadUnitString("test", source);

	// Call function with integers.
	auto ix = moon::CValue::MakeInteger(1);
	auto iy = moon::CValue::MakeInteger(1);
	auto iResult = engine.CallFunction("test", "module", { ix, iy });
	assert(iResult.GetType() == moon::EValueType::INTEGER);
	std::printf("Integer result : %ld\n", iResult.GetInteger());

	// Call function with reals.
	auto rx = moon::CValue::MakeReal(1);
	auto ry = moon::CValue::MakeReal(1);
	auto rResult = engine.CallFunction("test", "module", { rx, ry });
	assert(rResult.GetType() == moon::EValueType::REAL);
	std::printf("Real result : %f\n", rResult.GetReal());
}

int main()
{
	SimpleTest();
	std::printf("Hello, World!\n");
	return 0;
}
