#ifndef MOON_EXCEPT_COMPILATION_ERROR_H
#define MOON_EXCEPT_COMPILATION_ERROR_H

#include <string>
#include <sstream>

namespace moon {
namespace except {

	class ExCompilationError {
		int m_line;
		int m_column;
		const std::string m_innerMessage;

	public:
		ExCompilationError(int line, int column, const std::string& innerMessage) :
			m_line(line),
			m_column(column),
			m_innerMessage(innerMessage)
		{}

		std::string What() const
		{
			std::ostringstream oss;

			oss << "Compilation error @ (" << m_line << ":" << m_column << ") : "
				<< m_innerMessage;

			return oss.str();
		}
	};

}
}

#endif
