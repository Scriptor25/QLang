#include <map>
#include <memory>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>

QLang::StatementPtr QLang::Parser::ParseBinary()
{
    auto lhs = ParseOperand();
    if (!lhs) return {};
    return ParseBinary(std::move(lhs), 0);
}

static int get_precedence(const std::string& op)
{
    static std::map<std::string, int> precedences{
        {"?", 0},
        {"=", 0},
        {"+=", 0},
        {"-=", 0},
        {"*=", 0},
        {"/=", 0},
        {"%=", 0},
        {"<<=", 0},
        {">>=", 0},
        {">>>=", 0},
        {"&=", 0},
        {"^=", 0},
        {"|=", 0},

        {"||", 1},

        {"&&", 2},

        {"|", 3},

        {"^", 4},

        {"&", 5},

        {"==", 6}, {"!=", 6},

        {"<", 7}, {"<=", 7}, {">", 7}, {">=", 7},

        {"<<", 8}, {">>", 8}, {">>>", 8},

        {"+", 9}, {"-", 9},

        {"*", 10}, {"/", 10}, {"%", 10},
    };

    if (precedences.count(op)) return precedences[op];
    return -1;
}

QLang::StatementPtr QLang::Parser::ParseBinary(StatementPtr lhs, const size_t min_pre)
{
    while (At(TokenType_Operator) && get_precedence(m_State.Tok.Value) >= min_pre)
    {
        auto [Where, Type, Value] = Skip();
        const auto pre = get_precedence(Value);

        auto rhs = ParseOperand();
        if (!rhs) return {};
        while (At(TokenType_Operator) && get_precedence(m_State.Tok.Value) > pre)
        {
            const auto next_pre = get_precedence(m_State.Tok.Value);
            rhs = ParseBinary(std::move(rhs), pre + (next_pre > pre ? 1 : 0));
            if (!rhs) return {};
        }

        if (Value == "?")
        {
            Expect(":");
            auto else_ = ParseBinary();
            lhs = std::make_unique<TernaryExpression>(Where, std::move(lhs), std::move(rhs), std::move(else_));
            continue;
        }

        lhs = std::make_unique<BinaryExpression>(Where, Value, std::move(lhs), std::move(rhs));
    }

    return lhs;
}
