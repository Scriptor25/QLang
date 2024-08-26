#include <QLang/Expression.hpp>

QLang::UnaryExpression::UnaryExpression(
	const SourceLocation &where, const std::string &operator_,
	ExpressionPtr operand, bool left)
	: Expression(where), Operator(operator_), Operand(std::move(operand)),
	  Left(left)
{
}

void QLang::UnaryExpression::Print(std::ostream &stream) const
{
	if (Left) stream << Operator;
	Operand->Print(stream);
	if (!Left) stream << Operator;
}
