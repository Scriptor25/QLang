#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <iostream>
#include <utility>

QLang::CastExpression::CastExpression(
	const SourceLocation &where, const TypePtr &dst, StatementPtr src)
	: CastExpression(where, dst, dyn_cast<Expression>(src))
{
}

QLang::CastExpression::CastExpression(
	const SourceLocation &where, TypePtr dst, ExpressionPtr src)
	: Expression(where), Dst(std::move(dst)), Src(std::move(src))
{
}

bool QLang::CastExpression::IsConstant() const { return Src->IsConstant(); }

std::ostream &QLang::CastExpression::Print(std::ostream &stream) const
{
	return stream << "cast(" << Dst << ") (" << Src << ')';
}

QLang::ValuePtr QLang::CastExpression::GenIR(Builder &builder) const
{
	const auto src = Src->GenIR(builder);
	if (!src)
	{
		std::cerr << "    at " << Where << std::endl;
		return {};
	}
	return GenCast(builder, src, Dst);
}

QLang::ExpressionPtr QLang::CastExpression::Compress() { return {}; }
