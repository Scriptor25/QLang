#include "QLang/Operator.hpp"
#include <QLang/Expression.hpp>
#include <iostream>

QLang::CastExpression::CastExpression(
	const SourceLocation &where, const TypePtr &dst, ExpressionPtr src)
	: Expression(where), Dst(dst), Src(std::move(src))
{
}

std::ostream &QLang::CastExpression::Print(std::ostream &stream) const
{
	return stream << "cast(" << Dst << ") " << Src;
}

QLang::ValuePtr QLang::CastExpression::GenIR(Builder &builder) const
{
	auto src = Src->GenIR(builder);
	if (!src)
	{
		std::cerr << "    at " << Where << std::endl;
		return {};
	}
	return GenCast(builder, src, Dst);
}
