#include <QLang/Expression.hpp>
#include <QLang/Statement.hpp>
#include <iostream>

QLang::WhileStatement::WhileStatement(
	const SourceLocation &where, ExpressionPtr condition, StatementPtr loop)
	: Statement(where), Condition(std::move(condition)), Loop(std::move(loop))
{
}

void QLang::WhileStatement::Print(std::ostream &stream) const
{
	stream << "while ";
	Condition->Print(stream);
	stream << ' ';
	Loop->Print(stream);
}
