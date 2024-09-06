#include <memory>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>

QLang::StatementPtr QLang::Parser::ParseIf()
{
    auto where = Expect("if").Where;

    auto if_ = ParseBinary();
    auto then = ParseStatement();

    StatementPtr else_;
    if (NextIfAt("else")) else_ = ParseStatement();

    return std::make_unique<IfStatement>(where, std::move(if_), std::move(then), std::move(else_));
}
