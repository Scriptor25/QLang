#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>
#include <memory>

QLang::StatementPtr QLang::Parser::ParseIf()
{
	auto where = Expect("if").Where;

	auto if_ = ParseBinary();
	auto then = Parse();

	StatementPtr else_;
	if (NextIfAt("else")) else_ = Parse();

	return std::make_unique<IfStatement>(
		where, std::move(if_), std::move(then), std::move(else_));
}
