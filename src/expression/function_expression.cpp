#include <iostream>
#include <utility>
#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Value.hpp>

QLang::FunctionExpression::FunctionExpression(
    const SourceLocation& where,
    const std::vector<Param>& params,
    const bool vararg,
    TypePtr result,
    StatementPtr body)
    : Expression(where), Params(params), VarArg(vararg), Result(std::move(result)), Body(std::move(body))
{
}

std::ostream& QLang::FunctionExpression::Print(std::ostream& stream) const
{
    stream << "%(";
    for (size_t i = 0; i < Params.size(); ++i)
    {
        if (i > 0) stream << ", ";
        stream << Params[i].Type;
        if (!Params[i].Name.empty())
            stream << ' ' << Params[i].Name;
    }
    if (VarArg)
    {
        if (!Params.empty()) stream << ", ";
        stream << '?';
    }
    return stream << ") -> " << Result << ' ' << Body;
}

QLang::ValuePtr QLang::FunctionExpression::GenIR(Builder& builder) const
{
    if (const auto func = builder.CreateFunction(Where, FnMode_Func, Result, {}, {}, {}, Params, VarArg, Body.get()))
    {
        auto value = func->AsValue(builder);
        delete func;
        return value;
    }

    return {};
}

QLang::ExpressionPtr QLang::FunctionExpression::Collapse()
{
    return {};
}
