#ifndef MOON_COMMON_STACK_H
#define MOON_COMMON_STACK_H

#include <vector>

#include "../common/Log.h"
#include "StackFrame.h"

namespace moon {
namespace itpr {

	class CStack {
		std::vector<CStackFrame> m_calls;

	public:
		void PushCall(const std::string& function)
		{
			m_calls.emplace_back(function);
			LOG_TRACE("Pushed (%s)", m_calls.back().GetFunction().c_str());
		}

		void PopCall()
		{
			LOG_TRACE("Popping (%s)", m_calls.back().GetFunction().c_str());
			m_calls.pop_back();
		}

		template<class Func>
		void VisitCalls(Func func) const
		{
			for (unsigned i = 0; i < m_calls.size(); ++i) {
				func(i, m_calls[m_calls.size() - 1 - i]);
			}
		}
	};

}
}

#endif
