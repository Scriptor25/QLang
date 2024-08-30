#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <memory>

QLang::ExpressionPtr QLang::Parser::ParseUnary(ExpressionPtr operand)
{
	auto [Where, Type, Value] = Skip();

	return std::make_unique<UnaryExpression>(
		Where, Value, std::move(operand), true);
}
