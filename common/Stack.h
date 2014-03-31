#ifndef MOON_COMMON_STACK_H
#define MOON_COMMON_STACK_H

#include <vector>

#include "StackFrame.h"

namespace moon {
namespace common {

	class CStack {
		std::vector<CStackFrame> m_impl;
	public:
		void Push(const std::string& function)
		{
			m_impl.emplace_back(function);
		}

		template<class Func>
		void visit(Func func)
		{
			for (int i = 0; i < m_impl.size(); ++i) {
				func(i, m_impl[m_impl.size() - 1 - i]);
			}
		}
	};

}
}

#endif