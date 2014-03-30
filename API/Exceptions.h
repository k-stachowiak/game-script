#ifndef MOON_EXCEPTIONS_H
#define MOON_EXCEPTIONS_H

#include <stdexcept>
#include <sstream>
#include <string>

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

	class ExCompilationError : public std::runtime_error {

		static std::string BuildMessage(int line, int column, const std::string& message)
		{
			std::ostringstream oss;
			oss << "Compilation error @ (" << line << ":" << column << ") : " << message;
			return oss.str();
		}

	public:
		ExCompilationError(int line, int column, const std::string& innerMessage) :
			std::runtime_error{ BuildMessage(line, column, innerMessage) }
		{}
	};
}

#endif