#include <QLang/Expression.hpp>
#include <iostream>

QLang::MemberExpression::MemberExpression(
	const SourceLocation &where, ExpressionPtr object, bool dereference,
	const std::string &member)
	: Expression(where), Object(std::move(object)), Dereference(dereference),
	  Member(member)
{
}

std::ostream &QLang::MemberExpression::Print(std::ostream &stream) const
{
	return stream << Object << (Dereference ? '!' : '.') << Member;
}

QLang::ValuePtr QLang::MemberExpression::GenIR(Builder &builder) const
{
	std::cerr << "TODO: QLang::MemberExpression::GenIR" << std::endl;
	return {};
}
