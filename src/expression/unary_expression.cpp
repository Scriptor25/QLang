#include <QLang/Expression.hpp>
#include <iostream>

QLang::UnaryExpression::UnaryExpression(
	const SourceLocation &where, const std::string &operator_,
	ExpressionPtr operand, bool left)
	: Expression(where), Operator(operator_), Operand(std::move(operand)),
	  Left(left)
{
}

std::ostream &QLang::UnaryExpression::Print(std::ostream &stream) const
{
	return stream << (Left ? Operator : "") << Operand
				  << (Left ? "" : Operator);
}

QLang::ValuePtr QLang::UnaryExpression::GenIR(Builder &builder) const
{
	std::cerr << "at " << Where << ": TODO" << std::endl;
	return {};
}
