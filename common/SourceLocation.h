#ifndef MOON_SOURCE_LOCATION_H
#define MOON_SOURCE_LOCATION_H

namespace moon {

class SourceLocation {
    bool m_externalInvoke = false;
    bool m_builtInFunction = false;
    int m_line = -2;
    int m_column = -2;

    SourceLocation(
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
    SourceLocation() = default;
    SourceLocation(const SourceLocation&) = default;

    static SourceLocation MakeExternalInvoke()
    {
        return SourceLocation(true, false, -1, -1);
    }

    static SourceLocation MakeBuiltInFunction()
    {
        return SourceLocation(false, true, -1, -1);
    }

    static SourceLocation MakeRegular(int line, int column)
    {
        return SourceLocation(false, false, line, column);
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
