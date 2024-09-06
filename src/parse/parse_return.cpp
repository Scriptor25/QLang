#include <memory>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>

QLang::StatementPtr QLang::Parser::ParseReturn()
{
    auto where = Expect("return").Where;

    StatementPtr value;
    if (!NextIfAt("void")) value = ParseBinary();

    return std::make_unique<ReturnStatement>(where, std::move(value));
}
