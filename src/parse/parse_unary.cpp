#include <memory>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>

QLang::StatementPtr QLang::Parser::ParseUnary(StatementPtr operand)
{
    auto [Where, Type, Value] = Skip();

    return std::make_unique<UnaryExpression>(Where, Value, std::move(operand), true);
}
