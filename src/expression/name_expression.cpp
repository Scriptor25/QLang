#include <iostream>
#include <utility>
#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::NameExpression::NameExpression(const SourceLocation& where, std::string name)
    : Expression(where), Name(std::move(name))
{
}

std::ostream& QLang::NameExpression::Print(std::ostream& stream) const
{
    return stream << Name;
}

QLang::ValuePtr QLang::NameExpression::GenIR(Builder& builder) const
{
    if (builder.IsCallee())
    {
        builder.ClearCallee();

        if (Name == "self")
        {
            const auto self = builder["self"]->GetType();
            if (const auto func = builder.FindConstructor(self, builder.GetArgs()))
                return func->AsValue(builder);

            std::cerr << "at " << Where << ": no constructor for type " << self << " with args";
            for (const auto& arg : builder.GetArgs()) std::cerr << ' ' << arg;
            std::cerr << std::endl;

            return {};
        }

        if (auto& sym = builder[Name])
        {
            if (sym->GetType()->IsFunctionPointer())
            {
                if (const auto type = FunctionType::FromPtr(sym->GetType());
                    type->GetParamCount() == builder.GetArgCount()
                    || (type->IsVarArg()
                        && type->GetParamCount() < builder.GetArgCount()))
                {
                    size_t i = 0;
                    for (; i < type->GetParamCount(); ++i)
                        if (type->GetParam(i) != builder.GetArg(i)) break;
                    if (i == type->GetParamCount()) return sym;
                }
            }
        }

        if (const auto self = builder.GetContext().GetType(Name))
            if (const auto func
                = builder.FindConstructor(self, builder.GetArgs()))
                return func->AsValue(builder);

        if (const auto func = builder.FindFunction(Name, {}, builder.GetArgs()))
            return func->AsValue(builder);

        std::cerr << "at " << Where << ": no function with name '" << Name << "' and args";
        for (const auto& arg : builder.GetArgs()) std::cerr << ' ' << arg;
        std::cerr << std::endl;

        return {};
    }

    if (auto& sym = builder[Name]) return sym;
    if (const auto func = builder.FindFunction(Name)) return func->AsValue(builder);

    std::cerr << "at " << Where << ": no symbol with name '" << Name << "'" << std::endl;
    return {};
}

QLang::ExpressionPtr QLang::NameExpression::Collapse() { return {}; }
