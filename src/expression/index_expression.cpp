#include <QLang/Expression.hpp>
#include <iostream>

QLang::IndexExpression::IndexExpression(
	const SourceLocation &where, ExpressionPtr array, ExpressionPtr index)
	: Expression(where), Array(std::move(array)), Index(std::move(index))
{
}

std::ostream &QLang::IndexExpression::Print(std::ostream &stream) const
{
	return stream << Array << '[' << Index << ']';
}

QLang::ValuePtr QLang::IndexExpression::GenIR(Builder &builder) const
{
	std::cerr << "TODO: QLang::IndexExpression::GenIR" << std::endl;
	return {};
}
