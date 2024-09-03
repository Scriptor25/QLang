#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>

QLang::StatementPtr QLang::Parser::ParseMember(ExpressionPtr object)
{
	auto [Where, Type, Value] = Skip();
	auto [MWhere, MType, MValue] = Expect(TokenType_Name);

	auto member = std::make_unique<NameExpression>(MWhere, MValue);
	return std::make_unique<BinaryExpression>(
		Where, Value, std::move(object), std::move(member));
}
