#ifndef MOON_COMMON_STACK_FRAME_H
#define MOON_COMMON_STACK_FRAME_H

#include <string>

namespace moon {
namespace common {

	class CStackFrame {
		const std::string m_function;
	public:
		CStackFrame(std::string function) :
			m_function{ function }
		{}

		const std::string GetFunction() const
		{
			return m_function;
		}
	};

}
}

#endif