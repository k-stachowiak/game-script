#ifndef MOON_COMMON_STACK_H
#define MOON_COMMON_STACK_H

#include <vector>

#include "../common/Log.h"
#include "StackFrame.h"

namespace moon {
namespace itpr {

	class CStack {
		std::vector<CStackFrame> m_impl;
	public:
		void Push(const std::string& function)
		{
			m_impl.emplace_back(function);
			LOG_TRACE("Pushed (%s)", m_impl.back().GetFunction().c_str());
		}

		void Pop()
		{
			LOG_TRACE("Popping (%s)", m_impl.back().GetFunction().c_str());
			m_impl.pop_back();
		}

		template<class Func>
		void Visit(Func func) const
		{
			for (unsigned i = 0; i < m_impl.size(); ++i) {
				func(i, m_impl[m_impl.size() - 1 - i]);
			}
		}
	};

}
}

#endif
