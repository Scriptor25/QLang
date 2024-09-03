#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <map>
#include <memory>

QLang::StatementPtr QLang::Parser::ParseBinary()
{
	return ParseBinary(ParseOperand(), 0);
}

static int get_precedence(const std::string &op)
{
	static std::map<std::string, int> precedences{
		{ "?", 0 },	 { "=", 0 },  { "<<=", 0 }, { ">>=", 0 }, { ">>>=", 0 },
		{ "+=", 0 }, { "-=", 0 }, { "*=", 0 },	{ "/=", 0 },  { "%=", 0 },
		{ "&=", 0 }, { "|=", 0 }, { "^=", 0 },

		{ "||", 1 },

		{ "&&", 2 },

		{ "|", 3 },

		{ "^", 4 },

		{ "&", 5 },

		{ "==", 6 }, { "!=", 6 },

		{ "<", 7 },	 { "<=", 7 }, { ">", 7 },	{ ">=", 7 },

		{ "<<", 8 }, { ">>", 8 }, { ">>>", 8 },

		{ "+", 9 },	 { "-", 9 },

		{ "*", 10 }, { "/", 10 }, { "%", 10 },
	};

	if (precedences.count(op)) return precedences[op];
	return -1;
}

QLang::StatementPtr QLang::Parser::ParseBinary(
	StatementPtr lhs, size_t min_prec)
{
	while (At(TokenType_Operator) && get_precedence(m_Token.Value) >= min_prec)
	{
		auto [Where, Type, Value] = Skip();
		auto prec = get_precedence(Value);

		auto rhs = dynamic_pointer_cast<Expression>(ParseOperand());
		if (!rhs) return {};
		while (At(TokenType_Operator) && get_precedence(m_Token.Value) >= prec)
		{
			auto next_prec = get_precedence(m_Token.Value);
			rhs = dynamic_pointer_cast<Expression>(
				ParseBinary(std::move(rhs), prec + (next_prec > prec ? 1 : 0)));
			if (!rhs) return {};
		}

		if (Value == "?")
		{
			Expect(":");
			auto else_ = dynamic_pointer_cast<Expression>(ParseBinary());
			lhs = std::make_unique<TernaryExpression>(
				Where, dynamic_pointer_cast<Expression>(std::move(lhs)),
				std::move(rhs), std::move(else_));
			continue;
		}

		lhs = std::make_unique<BinaryExpression>(
			Where, Value, dynamic_pointer_cast<Expression>(std::move(lhs)),
			std::move(rhs));
	}

	return lhs;
}
