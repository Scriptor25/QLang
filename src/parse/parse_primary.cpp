#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <memory>
#include <stdexcept>
#include <string>

QLang::ExpressionPtr QLang::Parser::ParsePrimary()
{
	if (NextIfAt("("))
	{
		auto ptr = ParseBinary();
		Expect(")");
		return ptr;
	}

	if (At(TokenType_Operator))
	{
		auto [Where, Type, Value] = Skip();
		auto operand = ParseOperand();
		return std::make_unique<UnaryExpression>(
			Where, Value, std::move(operand), true);
	}

	if (At(TokenType_Name))
	{
		auto [Where, Type, Value] = Skip();
		return std::make_unique<NameExpression>(Where, Value);
	}

	if (At(TokenType_BinInt))
	{
		auto [Where, Type, Value] = Skip();
		auto value = std::stoull(Value, nullptr, 2);
		return std::make_unique<ConstIntExpression>(Where, value);
	}
	if (At(TokenType_OctInt))
	{
		auto [Where, Type, Value] = Skip();
		auto value = std::stoull(Value, nullptr, 8);
		return std::make_unique<ConstIntExpression>(Where, value);
	}
	if (At(TokenType_DecInt))
	{
		auto [Where, Type, Value] = Skip();
		auto value = std::stoull(Value);
		return std::make_unique<ConstIntExpression>(Where, value);
	}
	if (At(TokenType_HexInt))
	{
		auto [Where, Type, Value] = Skip();
		auto value = std::stoull(Value, nullptr, 16);
		return std::make_unique<ConstIntExpression>(Where, value);
	}

	if (At(TokenType_String))
	{
		auto [Where, Type, Value] = Skip();
		return std::make_unique<ConstStringExpression>(Where, Value);
	}

	throw std::runtime_error("unhandled token");
}
