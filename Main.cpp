#include <cstdio>
#include <cassert>
#include <cmath>

#include <map>
#include <string>
#include <functional>

#include "API/MoonEngine.h"
#include "API/Exceptions.h"

/* TODO:
 * - Store all values on stack.
 */

bool IsClose(double x, double y)
{
    return std::abs(x - y) < 0.01;
}

std::vector<std::pair<std::string, std::function<void()>>> assertions {

    // Interpretation errors.
    // ----------------------

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

    { "Another dom builder failure", []() {
        moon::CEngine engine;

        std::string source = "(bind one 1))";

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

    { "Simple char parsing", []() {

        moon::CEngine engine;

        try {
            std::string charParse = "(bind c 'c')";
            engine.LoadUnitString("test", charParse);
            moon::CValue result = engine.GetValue("test", "c");
            assert(result.GetType() == moon::EValueType::CHARACTER);
            assert(result.GetCharacter() == 'c');
        } catch (const moon::ExParsingError&) {
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

    { "Currying example", []() {

        std::string source =
            "(bind test(func(a b)\n"
            "\t(bind add_to_a (+ a))\n"
            "\t(add_to_a b)\n"
            "))";

        moon::CEngine engine;
        engine.LoadUnitString("test", source);

        auto a = moon::CValue::MakeInteger(1);
        auto b = moon::CValue::MakeInteger(2);
        auto result = engine.CallFunction("test", "test", { a, b });

        assert(result.GetType() == moon::EValueType::INTEGER);
        assert(result.GetInteger() == 3);
    } },

    { "Function reference evaluated", []() {
        std::string source =
            "(bind f (func (x) x))\n"
            "(bind symbol f)";

        moon::CEngine engine;

        try {
            engine.LoadUnitString("test", source);
            engine.GetValue("test", "symbol");
            assert(false);
        } catch (const moon::ExValueRequestedFromFuncBind&) {
        }
    } },

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
    } },

    { "Symbol shadowing example.", []() {

        std::string source =
            "(bind x 2)\n"
            "(bind dblx (func () (* x 2)))\n"
            "(bind scope-test (func ()\n"
            "\t(bind x 100)\n"
            "\t(dblx)\n"
            "))";

        moon::CEngine engine;
        engine.LoadUnitString("test", source);

        auto result = engine.CallFunction("test", "scope-test", {});

        assert(result.GetType() == moon::EValueType::INTEGER);
        assert(result.GetInteger() == 4);
    } },

};

std::vector<std::pair<std::string, std::function<bool()>>> optionals{

    { "Local scope capture.", []() {
        std::string source =
            "(bind lhs 1)\n"
            "(bind make_doubler (func ()\n"
            "\t(bind lhs 2)\n"
            "\t(func (rhs)\n"
            "\t\t(* lhs rhs)\n"
            "\t)\n"
            "))\n"
            "(bind test(func(rhs)\n"
            "\t(bind doubler(make_doubler))\n"
            "\t(doubler rhs)\n"
            "))";

        moon::CEngine engine;
        engine.LoadUnitString("test", source);

        auto result = engine.CallFunction("test", "test", { moon::CValue::MakeInteger(3) });

        assert(result.GetType() == moon::EValueType::INTEGER);

        if (result.GetInteger() == 3) {
            return false;
        }
        else if (result.GetInteger() == 6) {
            return true;
        }
        else {
            assert(false);
            return false;
        }
    } },

};

int main()
{
    std::printf("Assertions:\n");
    for (const auto& pr : assertions) {
        std::printf("%s... ", pr.first.c_str());
        pr.second();
        std::printf("Passed\n");
    }

    std::printf("\n");

    std::printf("Checks:\n");
    for (const auto& pr : optionals) {
        std::printf("%s... ", pr.first.c_str());
        if (pr.second()) {
            std::printf("Yes\n");
        } else {
            std::printf("No\n");
        }
    }

    return 0;
}
