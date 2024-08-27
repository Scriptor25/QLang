#include <QLang/Expression.hpp>
#include <iostream>

QLang::NameExpression::NameExpression(
	const SourceLocation &where, const std::string &name)
	: Expression(where), Name(name)
{
}

std::ostream &QLang::NameExpression::Print(std::ostream &stream) const
{
	return stream << Name;
}

QLang::ValuePtr QLang::NameExpression::GenIR(Builder &builder) const
{
	std::cerr << "TODO: QLang::NameExpression::GenIR" << std::endl;
	return {};
}
