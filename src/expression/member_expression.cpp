#include <QLang/Expression.hpp>
#include <iostream>

QLang::MemberExpression::MemberExpression(
	const SourceLocation &where, ExpressionPtr object, bool dereference,
	const std::string &member)
	: Expression(where), Object(std::move(object)), Dereference(dereference),
	  Member(member)
{
}

void QLang::MemberExpression::Print(std::ostream &stream) const
{
	Object->Print(stream);
	stream << (Dereference ? '!' : '.') << Member;
}
