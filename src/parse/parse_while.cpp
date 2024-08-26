#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>
#include <memory>

QLang::StatementPtr QLang::Parser::ParseWhile()
{
	auto where = Expect("while").Where;

	auto condition = ParseBinary();
	auto loop = Parse();

	return std::make_unique<WhileStatement>(
		where, std::move(condition), std::move(loop));
}
