#include <QLang/Expression.hpp>
#include <iostream>

QLang::BinaryExpression::BinaryExpression(
	const SourceLocation &where, const std::string &operator_,
	ExpressionPtr lhs, ExpressionPtr rhs)
	: Expression(where), Operator(operator_), LHS(std::move(lhs)),
	  RHS(std::move(rhs))
{
}

void QLang::BinaryExpression::Print(std::ostream &stream) const
{
	LHS->Print(stream);
	stream << ' ' << Operator << ' ';
	RHS->Print(stream);
}
