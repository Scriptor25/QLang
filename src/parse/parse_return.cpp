#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>
#include <memory>

QLang::StatementPtr QLang::Parser::ParseReturn()
{
	auto where = Expect("return").Where;

	ExpressionPtr value;
	if (!NextIfAt("void")) value = ParseBinary();

	return std::make_unique<ReturnStatement>(where, std::move(value));
}
