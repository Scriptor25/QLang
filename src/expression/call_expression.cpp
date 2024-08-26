#include <QLang/Expression.hpp>
#include <iostream>

QLang::CallExpression::CallExpression(
	const SourceLocation &where, ExpressionPtr callee,
	std::vector<ExpressionPtr> &args)
	: Expression(where), Callee(std::move(callee)), Args(std::move(args))
{
}

void QLang::CallExpression::Print(std::ostream &stream) const
{
	Callee->Print(stream);
	stream << '(';
	for (size_t i = 0; i < Args.size(); ++i)
	{
		if (i > 0) stream << ", ";
		Args[i]->Print(stream);
	}
	stream << ')';
}
