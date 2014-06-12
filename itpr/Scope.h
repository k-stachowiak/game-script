#ifndef MOON_ITPR_SCOPE_H
#define MOON_ITPR_SCOPE_H

#include <memory>
#include <string>
#include <utility>
#include <map>

#include "Stack.h"

#include "Value.h"
#include "../common/SourceLocation.h"
#include "../ast/Bind.h"

namespace moon {
namespace itpr {

    class GlobalScope;

    // TODO: this is identical to the SCapture. Merge the two?
    struct ValueStore {
        Value value;
        SourceLocation location;
    };

    class Scope {
    protected:
        std::map<std::string, ValueStore> t_binds;
    public:
        virtual ~Scope() {}

        virtual const ValueStore GetValueStore(
            const std::string& name,
            const SourceLocation& location,
            const Stack& stack) const = 0;

        virtual GlobalScope& GetGlobalScope() = 0;

        virtual std::map<std::string, Capture>
        CaptureNonGlobals(const std::vector<std::string>& names) const = 0;

        void TryRegisteringBind(
            const Stack& stack,
            const std::string& name,
            const Value& value,
            const SourceLocation& location);

        std::vector<std::string> GetAllValues() const;
        std::vector<std::string> GetAllFunctions() const;

        Value GetValue(
            const std::string& name,
            const SourceLocation& location,
            const Stack& stack);
    };

    class GlobalScope : public Scope {
    public:
        GlobalScope& GetGlobalScope() override { return *this; }

        const ValueStore GetValueStore(
            const std::string& name,
            const SourceLocation& location,
            const Stack& stack) const override;

        std::map<std::string, Capture>
        CaptureNonGlobals(const std::vector<std::string>&) const override
        {
            return {};
        }
    };

    class LocalScope : public Scope {
        GlobalScope& m_globalScope;

    public:
        LocalScope(GlobalScope& globalScope) : m_globalScope(globalScope) {}
        
        GlobalScope& GetGlobalScope() override { return m_globalScope; }

        const ValueStore GetValueStore(
            const std::string& name,
            const SourceLocation& location,
            const Stack& stack) const override;

        std::map<std::string, Capture>
        CaptureNonGlobals(const std::vector<std::string>& names) const override;
    };


}
}

#endif
