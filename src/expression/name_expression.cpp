#include <QLang/Expression.hpp>

QLang::NameExpression::NameExpression(
	const SourceLocation &where, const std::string &name)
	: Expression(where), Name(name)
{
}

void QLang::NameExpression::Print(std::ostream &stream) const
{
	stream << Name;
}
