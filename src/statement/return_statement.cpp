#include <QLang/Expression.hpp>
#include <QLang/Statement.hpp>
#include <iostream>

QLang::ReturnStatement::ReturnStatement(
	const SourceLocation &where, ExpressionPtr value)
	: Statement(where), Value(std::move(value))
{
}

void QLang::ReturnStatement::Print(std::ostream &stream) const
{
	stream << "return ";
	if (Value) Value->Print(stream);
	else
		stream << "void";
}
