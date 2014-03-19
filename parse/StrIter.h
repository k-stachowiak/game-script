#ifndef MOON_COMMON_STRITER_H
#define MOON_COMMON_STRITER_H

#include <iterator>
#include <string>

namespace moon {
namespace parse {

	class CStrIter : public std::iterator<std::bidirectional_iterator_tag, char>
	{
		const char* m_begin;
		const char* m_current;

		CStrIter(const char* current) :
			m_begin(current),
			m_current(current)
		{}

	public:
		friend bool operator==(const CStrIter& lhs, const CStrIter& rhs)
		{
			return lhs.m_current == rhs.m_current;
		}

		friend bool operator!=(const CStrIter& lhs, const CStrIter& rhs)
		{
			return !(lhs == rhs);
		}

		CStrIter& operator=(const CStrIter& other)
		{
			m_begin = other.m_begin;
			m_current = other.m_current;
			return *this;
		}

		const char& operator*() const
		{
			return *m_current;
		}

		CStrIter& operator++()
		{
			++m_current;
			return *this;
		}

		CStrIter& operator--()
		{
			--m_current;
			return *this;
		}

		operator const char*() const
		{
			return m_current;
		}

		friend CStrIter operator+(CStrIter iter, int increment)
		{
			std::advance(iter.m_current, increment);
			return iter;
		}

		friend CStrIter operator-(CStrIter iter, int decrement)
		{
			std::advance(iter.m_current, -decrement);
			return iter;
		}

		static CStrIter Begin(const std::string& str)
		{
			return CStrIter{ str.data() };
		}

		static CStrIter End(const std::string& str)
		{
			return CStrIter{ str.data() + str.size() };
		}
	};

}
}

#endif