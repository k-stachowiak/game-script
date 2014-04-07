#include <cstdio>
#include <cassert>
#include <cmath>

#include <map>
#include <string>
#include <functional>

#include "API/MoonEngine.h"
#include "API/Exceptions.h"

/*
 * Exceptions to test:
 * [v] Tokenizer
 * [v] Dom builder
 * [v] SexprParser
 * RUNTIME ERRORS...
 */

/* TODO:
 * - Proper handling of the function declaration evaluation - currying?
 * - Logical assertions about the AST, e.g. cannot bind a bind.
 * - Start building the documentation.
 */

bool IsClose(double x, double y)
{
	return std::abs(x - y) < 0.01;
}

std::vector<std::pair<std::string, std::function<void()>>> tests{

	// Interpretation errors.
	// ----------------------

	{ "Function reference evaluated", []() {
		std::string source =
			"(bind f (func (x) x))\n"
			"(bind symbol f)";

		moon::CEngine engine;

		try {
			engine.LoadUnitString("test", source);
			engine.GetValue("test", "symbol");
			assert(false);
		}
		catch (const moon::ExInterpretationError&) {
		}
	} },
	
	{ "Function passed as argument for numeric magic", []() {
		std::string source = "(bind four (+ 2 (func (x) 2)))";

		moon::CEngine engine;

		try {
			engine.LoadUnitString("test", source);
			engine.GetValue("test", "four");
			assert(false);
		} catch (const moon::ExInterpretationError&) {
		}
	} },

	{ "Vector construction failure", []() {

		std::string source =
			"(bind arr [\"one\" 2 3.0])";

		moon::CEngine engine;

		try {
			engine.LoadUnitString("test", source);
			engine.GetValue("test", "arr");
			assert(false);
		}
		catch (const moon::ExInterpretationError&) {
		}

	} },

	// Parsing errors.
	// ---------------

	{ "Parsing : toplevel non-bind expression", []() {
		moon::CEngine engine;

		std::string source = "(func sqr (x) (* x x))";

		try {
			engine.LoadUnitString("test", source);
			assert(false);
		} catch (const moon::ExParsingError&) {
		}
	} },

	{ "Dom builder failure", []() {
		moon::CEngine engine;

		std::string source = "(bind one 1";

		try {
			engine.LoadUnitString("test", source);
			assert(false);
		} catch (const moon::ExParsingError&) {
		}
	} },

	{ "Tokenization failure", []() {

		moon::CEngine engine;

		try {
			std::string unclosedString = "(bind unclosed-string \"asd)";
			engine.LoadUnitString("test", unclosedString);
			assert(false);
		}
		catch (const moon::ExParsingError&) {
		}

		try {
			std::string unclosedChar = "(bind unclosed-char \'c)";
			engine.LoadUnitString("test", unclosedChar);
			assert(false);
		}
		catch (const moon::ExParsingError&) {
		}

	} },

	// Runtime logic.
	// --------------

	{ "Constructing vector", []() {

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
	} },

	// Client scenarios.
	// -----------------

	{ "Simple function call", []() {

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
	} },

	{ "Simple value retrieval", []() {

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
	} }
};

int main()
{
	for (const auto& pr : tests) {
		std::printf("%s... ", pr.first.c_str());
		pr.second();
		std::printf("OK\n");
	}

	return 0;
}
