#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>

QLang::StatementPtr QLang::Parser::ParseOperand()
{
    auto ptr = ParsePrimary();

    while (ptr)
    {
        if (At("("))
        {
            ptr = ParseCall(std::move(ptr));
            continue;
        }

        if (At("["))
        {
            auto where = Skip().Where;
            auto index = ParseBinary();
            Expect("]");

            ptr = std::make_unique<BinaryExpression>(where, "[]", std::move(ptr), std::move(index));
            continue;
        }

        if (At(".") || At("!"))
        {
            auto [where_, type_, value_] = Skip();
            auto member = ParsePrimary();

            ptr = std::make_unique<BinaryExpression>(where_, value_, std::move(ptr), std::move(member));
            continue;
        }

        if (At("++") || At("--"))
        {
            auto [Where, Type, Value] = Skip();

            ptr = std::make_unique<UnaryExpression>(Where, Value, std::move(ptr), true);
            continue;
        }

        return ptr;
    }

    return {};
}
