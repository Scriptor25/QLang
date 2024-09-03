#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <memory>

QLang::StatementPtr QLang::Parser::ParseIndex(ExpressionPtr array)
{
	auto where = Expect("[").Where;
	auto index = dynamic_pointer_cast<Expression>(ParseBinary());
	Expect("]");

	return std::make_unique<BinaryExpression>(
		where, "[]", std::move(array), std::move(index));
}
