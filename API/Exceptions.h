#ifndef MOON_EXCEPTIONS_H
#define MOON_EXCEPTIONS_H

#include <stdexcept>
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

	struct ExUnitAlreadyRegisterend : public std::runtime_error {
		ExUnitAlreadyRegisterend(const std::string& unitName) :
			std::runtime_error{ "Unit already registered : \"" + unitName + "\"" }
		{}
	};
}

#endif