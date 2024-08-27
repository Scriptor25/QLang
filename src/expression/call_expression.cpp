#include <QLang/Expression.hpp>
#include <iostream>

QLang::CallExpression::CallExpression(
	const SourceLocation &where, ExpressionPtr callee,
	std::vector<ExpressionPtr> &args)
	: Expression(where), Callee(std::move(callee)), Args(std::move(args))
{
}

std::ostream &QLang::CallExpression::Print(std::ostream &stream) const
{
	stream << Callee << '(';
	for (size_t i = 0; i < Args.size(); ++i)
	{
		if (i > 0) stream << ", ";
		stream << Args[i];
	}
	return stream << ')';
}

QLang::ValuePtr QLang::CallExpression::GenIR(Builder &builder) const
{
	std::cerr << "TODO: QLang::CallExpression::GenIR" << std::endl;
	return {};
}
