#ifndef MOON_SOURCE_LOCATION_H
#define MOON_SOURCE_LOCATION_H

namespace moon {

class CSourceLocation {
	bool m_externalInvoke = false;
	bool m_builtInFunction = false;
	int m_line = -2;
	int m_column = -2;

	CSourceLocation(
		bool externalInvoke,
		bool builtInFunction,
		int line,
		int column) :
		m_externalInvoke(externalInvoke),
		m_builtInFunction(builtInFunction),
		m_line(line),
		m_column(column)
	{}

public:
	CSourceLocation() = default;
	CSourceLocation(const CSourceLocation&) = default;

	static CSourceLocation MakeExternalInvoke()
	{
		return CSourceLocation(true, false, -1, -1);
	}

	static CSourceLocation MakeBuiltInFunction()
	{
		return CSourceLocation(false, true, -1, -1);
	}

	static CSourceLocation MakeRegular(int line, int column)
	{
		return CSourceLocation(false, false, line, column);
	}

	bool IsRegular() const
	{
		return !m_externalInvoke && !m_builtInFunction;
	}

	int GetLine() const
	{
		return m_line;
	}

	int GetColumn() const
	{
		return m_column;
	}
};

}

#endif
