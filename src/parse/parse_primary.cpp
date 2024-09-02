#include <QLang/Context.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Token.hpp>
#include <iostream>
#include <memory>
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
			Where, Value, std::move(operand), false);
	}

	if (At(TokenType_Name))
	{
		auto [Where, Type, Value] = Skip();
		if (m_Context.HasMacro(Value) && !m_Context.GetMacro(Value).IsCallee)
			return dynamic_pointer_cast<Expression>(
				m_Context.GetMacro(Value).Resolve(*this));
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

	if (At(TokenType_Float))
	{
		auto [Where, Type, Value] = Skip();
		auto value = std::stod(Value);
		return std::make_unique<ConstFloatExpression>(Where, value);
	}

	if (At(TokenType_Char))
	{
		auto [Where, Type, Value] = Skip();
		return std::make_unique<ConstCharExpression>(Where, Value[0]);
	}

	if (At(TokenType_String))
	{
		auto [Where, Type, Value] = Skip();
		return std::make_unique<ConstStringExpression>(Where, Value);
	}

	auto [Where, Type, Value] = Skip();
	std::cerr << "at " << Where << ": unhandled token '" << Value << "' ("
			  << Type << ')' << std::endl;
	return {};
}
