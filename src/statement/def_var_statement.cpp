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

std::ostream &QLang::DefVarStatement::Print(std::ostream &stream) const
{
	if (!Init) return stream << "def " << Type << ' ' << Name;
	return stream << "def " << Type << ' ' << Name << " = " << Init;
}

void QLang::DefVarStatement::GenIRVoid(Builder &builder) const
{
	std::cerr << "TODO: QLang::DefVarStatement::GenIRVoid" << std::endl;
}
