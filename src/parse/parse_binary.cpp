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
        auto [where_, type_, value_] = Skip();
        const auto pre = get_precedence(value_);

        auto rhs = ParseOperand();
        if (!rhs) return {};
        while (At(TokenType_Operator) && get_precedence(m_State.Tok.Value) >= pre)
        {
            rhs = ParseBinary(std::move(rhs), pre ? pre + 1 : pre);
            if (!rhs) return {};
        }

        if (value_ == "?")
        {
            Expect(":");
            auto else_ = ParseBinary();
            lhs = std::make_unique<TernaryExpression>(where_, std::move(lhs), std::move(rhs), std::move(else_));
            continue;
        }

        lhs = std::make_unique<BinaryExpression>(where_, value_, std::move(lhs), std::move(rhs));
    }

    return lhs;
}
