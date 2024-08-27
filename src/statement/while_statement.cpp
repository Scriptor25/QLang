#include <QLang/Expression.hpp>
#include <QLang/Statement.hpp>
#include <iostream>

QLang::WhileStatement::WhileStatement(
	const SourceLocation &where, ExpressionPtr condition, StatementPtr loop)
	: Statement(where), Condition(std::move(condition)), Loop(std::move(loop))
{
}

std::ostream &QLang::WhileStatement::Print(std::ostream &stream) const
{
	return stream << "while " << Condition << ' ' << Loop;
}

void QLang::WhileStatement::GenIRVoid(Builder &builder) const
{
	std::cerr << "TODO: QLang::WhileStatement::GenIRVoid" << std::endl;
}
