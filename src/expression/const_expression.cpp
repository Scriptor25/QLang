#include <QLang/Expression.hpp>
#include <iostream>

QLang::ConstIntExpression::ConstIntExpression(
	const SourceLocation &where, uint64_t value)
	: Expression(where), Value(value)
{
}

void QLang::ConstIntExpression::Print(std::ostream &stream) const
{
	stream << Value;
}

QLang::ConstStringExpression::ConstStringExpression(
	const SourceLocation &where, const std::string &value)
	: Expression(where), Value(value)
{
}

void QLang::ConstStringExpression::Print(std::ostream &stream) const
{
	stream << '"' << Value << '"';
}
