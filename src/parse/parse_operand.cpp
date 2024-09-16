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
            ptr = ParseIndex(std::move(ptr));
            continue;
        }

        if (At("++") || At("--"))
        {
            ptr = ParseUnary(std::move(ptr));
            continue;
        }

        return ptr;
    }

    return {};
}
