#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <map>

QLang::ExpressionPtr QLang::Parser::ParseBinary()
{
	return ParseBinary(ParseOperand(), 0);
}

static int get_precedence(const std::string &op)
{
	static std::map<std::string, int> precedences{
		{ "=", 0 },	 { "<<=", 0 }, { ">>=", 0 }, { ">>>=", 0 },
		{ "+=", 0 }, { "-=", 0 },  { "*=", 0 },	 { "/=", 0 },
		{ "%=", 0 }, { "&=", 0 },  { "|=", 0 },	 { "^=", 0 },

		{ "||", 1 },

		{ "&&", 2 },

		{ "|", 3 },

		{ "^", 4 },

		{ "&", 5 },

		{ "==", 6 }, { "!=", 6 },

		{ "<", 7 },	 { "<=", 7 },  { ">", 7 },	 { ">=", 7 },

		{ "<<", 8 }, { ">>", 8 },  { ">>>", 8 },

		{ "+", 9 },	 { "-", 9 },

		{ "*", 10 }, { "/", 10 },  { "%", 10 },
	};

	if (precedences.count(op)) return precedences[op];
	return -1;
}

QLang::ExpressionPtr QLang::Parser::ParseBinary(
	ExpressionPtr lhs, size_t min_prec)
{
	while (At(TokenType_Operator) && get_precedence(m_Token.Value) >= min_prec)
	{
		auto [Where, Type, Value] = Skip();
		auto prec = get_precedence(Value);

		auto rhs = ParseOperand();
		if (!rhs) return {};
		while (At(TokenType_Operator) && get_precedence(m_Token.Value) >= prec)
		{
			auto next_prec = get_precedence(m_Token.Value);
			rhs = ParseBinary(
				std::move(rhs), prec + (next_prec > prec ? 1 : 0));
			if (!rhs) return {};
		}

		lhs = std::make_unique<BinaryExpression>(
			Where, Value, std::move(lhs), std::move(rhs));
	}

	return lhs;
}
