#include <vector>
#include <QLang/Context.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>

QLang::StatementPtr QLang::Parser::ParseCall(StatementPtr callee)
{
    auto where = Expect("(").Where;

    std::vector<StatementPtr> args;
    while (!NextIfAt(")"))
    {
        args.push_back(ParseBinary());
        if (!At(")")) Expect(",");
    }

    if (auto name = dyn_cast<NameExpression>(callee))
    {
        if (m_Context.HasMacro(name->Name))
        {
            if (auto& [_where, _name, _params, _is_callee, _value] = m_Context.GetMacro(name->Name);
                _is_callee)
            {
                m_Context.GetMacro(name->Name).Expand(*this, args);
                return ParseOperand();
            }
        }

        callee = std::move(name);
    }

    return std::make_unique<CallExpression>(where, std::move(callee), std::move(args));
}
