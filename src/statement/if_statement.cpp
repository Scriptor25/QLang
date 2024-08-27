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

std::ostream &QLang::IfStatement::Print(std::ostream &stream) const
{
	if (!Else) return stream << "if " << If << ' ' << Then;
	return stream << "if " << If << ' ' << Then << " else " << Else;
}

void QLang::IfStatement::GenIRVoid(Builder &builder) const
{
	std::cerr << "TODO: QLang::IfStatement::GenIRVoid" << std::endl;
}
