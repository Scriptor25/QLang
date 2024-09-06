#include <memory>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>

QLang::StatementPtr QLang::Parser::ParseWhile()
{
    auto where = Expect("while").Where;

    auto while_ = ParseBinary();
    auto loop = ParseStatement();

    return std::make_unique<WhileStatement>(where, std::move(while_), std::move(loop));
}
