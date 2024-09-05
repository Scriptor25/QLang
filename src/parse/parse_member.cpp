#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>

QLang::StatementPtr QLang::Parser::ParseMember(StatementPtr object)
{
	auto [Where, Type, Value] = Skip();
	auto [_where, _type, _value] = Expect(TokenType_Name);

	auto member = std::make_unique<NameExpression>(_where, _value);
	return std::make_unique<BinaryExpression>(
		Where, Value, std::move(object), std::move(member));
}
