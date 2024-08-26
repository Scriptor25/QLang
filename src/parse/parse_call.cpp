#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <vector>

QLang::ExpressionPtr QLang::Parser::ParseCall(ExpressionPtr callee)
{
	auto where = Expect("(").Where;

	std::vector<ExpressionPtr> args;
	while (!NextIfAt(")"))
	{
		args.push_back(ParseBinary());
		if (!At(")")) Expect(",");
	}

	return std::make_unique<CallExpression>(where, std::move(callee), args);
}
