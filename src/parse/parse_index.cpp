#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <memory>

QLang::ExpressionPtr QLang::Parser::ParseIndex(ExpressionPtr array)
{
	auto where = Expect("[").Where;
	auto index = ParseBinary();
	Expect("]");

	return std::make_unique<BinaryExpression>(
		where, "[]", std::move(array), std::move(index));
}
