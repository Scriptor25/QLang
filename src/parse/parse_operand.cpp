#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>

QLang::StatementPtr QLang::Parser::ParseOperand()
{
	auto ptr = ParsePrimary();

	while (ptr)
	{
		if (At("("))
		{
			ptr = ParseCall(dynamic_pointer_cast<Expression>(std::move(ptr)));
			continue;
		}

		if (At("["))
		{
			ptr = ParseIndex(dynamic_pointer_cast<Expression>(std::move(ptr)));
			continue;
		}

		if (At(".") || At("!"))
		{
			ptr = ParseMember(dynamic_pointer_cast<Expression>(std::move(ptr)));
			continue;
		}

		if (At("++") || At("--"))
		{
			ptr = ParseUnary(dynamic_pointer_cast<Expression>(std::move(ptr)));
			continue;
		}

		return ptr;
	}

	return {};
}
