#include <QLang/Expression.hpp>
#include <QLang/Statement.hpp>
#include <iostream>

QLang::IfStatement::IfStatement(
	const SourceLocation &where, ExpressionPtr if_, StatementPtr then,
	StatementPtr else_)
	: Statement(where), If(std::move(if_)), Then(std::move(then)),
	  Else(std::move(else_))
{
}

void QLang::IfStatement::Print(std::ostream &stream) const
{
	stream << "if ";
	If->Print(stream);
	stream << ' ';
	Then->Print(stream);
	if (Else)
	{
		stream << " else ";
		Else->Print(stream);
	}
}
