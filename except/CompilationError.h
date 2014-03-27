#ifndef MOON_EXCEPT_COMPILATION_ERROR_H
#define MOON_EXCEPT_COMPILATION_ERROR_H

#include <string>
#include <sstream>

#include "../API/SourceLocation.h"

namespace moon {
namespace except {

	class ExCompilationError {
		const CSourceLocation m_location;
		const std::string m_innerMessage;

	public:
		ExCompilationError(CSourceLocation location, const std::string& innerMessage) :
			m_location(location),
			m_innerMessage(innerMessage)
		{}

		std::string What() const
		{
			std::ostringstream oss;

			oss << "Compilation error @ ("
				<< m_location.GetLine() << ":"
				<< m_location.GetColumn() << ") : "
				<< m_innerMessage;

			return oss.str();
		}
	};

}
}

#endif
