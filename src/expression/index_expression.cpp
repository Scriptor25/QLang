#include <QLang/Expression.hpp>
#include <iostream>

QLang::IndexExpression::IndexExpression(
	const SourceLocation &where, ExpressionPtr array, ExpressionPtr index)
	: Expression(where), Array(std::move(array)), Index(std::move(index))
{
}

void QLang::IndexExpression::Print(std::ostream &stream) const
{
	Array->Print(stream);
	stream << '[';
	Index->Print(stream);
	stream << ']';
}
