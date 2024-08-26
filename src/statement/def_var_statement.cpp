#include <QLang/Expression.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Type.hpp>
#include <iostream>

QLang::DefVarStatement::DefVarStatement(
	const SourceLocation &where, const TypePtr &type, const std::string &name,
	ExpressionPtr init)
	: Statement(where), Type(type), Name(name), Init(std::move(init))
{
}

void QLang::DefVarStatement::Print(std::ostream &stream) const
{
	stream << "def ";
	if (Type) stream << Type->GetName() << ' ';
	stream << Name;
	if (Init)
	{
		stream << " = ";
		Init->Print(stream);
	}
}
