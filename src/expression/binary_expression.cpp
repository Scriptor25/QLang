#include <QLang/Expression.hpp>
#include <iostream>

QLang::BinaryExpression::BinaryExpression(
	const SourceLocation &where, const std::string &operator_,
	ExpressionPtr lhs, ExpressionPtr rhs)
	: Expression(where), Operator(operator_), LHS(std::move(lhs)),
	  RHS(std::move(rhs))
{
}

std::ostream &QLang::BinaryExpression::Print(std::ostream &stream) const
{
	return stream << LHS << ' ' << Operator << ' ' << RHS;
}

QLang::ValuePtr QLang::BinaryExpression::GenIR(Builder &builder) const
{
	std::cerr << "TODO: QLang::BinaryExpression::GenIR" << std::endl;
	return {};
}
