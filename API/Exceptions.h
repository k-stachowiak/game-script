#ifndef MOON_EXCEPTIONS_H
#define MOON_EXCEPTIONS_H

#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace moon {

	struct ExFileNotFound : public std::runtime_error {
		ExFileNotFound(const std::string& path) :
			std::runtime_error{ "File not found : \"" + path + "\"" }
		{}
	};

	struct ExUnitNotRegistered : public std::runtime_error {
		ExUnitNotRegistered(const std::string& unitName) :
			std::runtime_error{ "Unit not registered : \"" + unitName + "\"" }
		{}
	};

	struct ExUnitAlreadyRegistered : public std::runtime_error {
		ExUnitAlreadyRegistered(const std::string& unitName) :
			std::runtime_error{ "Unit already registered : \"" + unitName + "\"" }
		{}
	};

	struct ExValueRequestedFromFuncBind : public std::runtime_error {
		ExValueRequestedFromFuncBind() :
			std::runtime_error{ "Value requested from a function bind." }
		{}
	};

	struct ExParsingError : public std::runtime_error {
		static std::string BuildMessage(
				int line,
				int column,
				const std::string& innerMessage)
		{
			std::ostringstream oss;
			oss << "Parser error @ (" << line << ":" << column << ") - " << innerMessage;
			return oss.str();
		}

		ExParsingError(int line, int column, const std::string& innerMessage) :
			std::runtime_error{ BuildMessage(line, column, innerMessage) }
		{}
	};

	struct ExInterpretationError : public std::runtime_error {
		static std::string BuildMessage(
				int line,
				int column,
				const std::vector<std::string>& stackFrames,
				const std::string& innerMessage)
		{
			std::ostringstream oss;
			oss << "Interpreter error @ (" << line << ":" << column << ")" << std::endl;
			oss << innerMessage << std::endl;
			for (const std::string& stackFrame : stackFrames) {
				oss << '\t' << stackFrame << std::endl;
			}
			return oss.str();
		}

		ExInterpretationError(
				int line,
				int column,
				const std::vector<std::string>& stackFrames,
				const std::string& innerMessage) :
				std::runtime_error{ BuildMessage(line, column, stackFrames, innerMessage) }
		{}
	};

}

#endif
