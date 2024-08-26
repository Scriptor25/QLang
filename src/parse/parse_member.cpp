#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>

QLang::ExpressionPtr QLang::Parser::ParseMember(ExpressionPtr object)
{
	SourceLocation where;
	auto dereference = At("!");

	if (!dereference) { where = Expect(".").Where; }
	else { where = Skip().Where; }

	auto member = Expect(TokenType_Name).Value;
	return std::make_unique<MemberExpression>(
		where, std::move(object), dereference, member);
}
