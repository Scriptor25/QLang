#include <iostream>
#include <utility>
#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>

QLang::CastExpression::CastExpression(
    const SourceLocation& where, const TypePtr& dst, StatementPtr src)
    : CastExpression(where, dst, dyn_cast<Expression>(src))
{
}

QLang::CastExpression::CastExpression(
    const SourceLocation& where, TypePtr dst, ExpressionPtr src)
    : Expression(where), Dst(std::move(dst)), Src(std::move(src))
{
}

std::ostream& QLang::CastExpression::Print(std::ostream& stream) const
{
    return stream << "cast(" << Dst << ") (" << Src << ')';
}

QLang::ValuePtr QLang::CastExpression::GenIR(Builder& builder) const
{
    const auto src = Src->GenIR(builder);
    if (!src) return {};
    return GenCast(Where, builder, src, Dst);
}

QLang::ExpressionPtr QLang::CastExpression::Collapse()
{
    if (auto src = Src->Collapse()) Src = std::move(src);
    return {};
}
