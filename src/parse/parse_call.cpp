#include <QLang/Context.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>
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

	if (auto name = dynamic_cast<NameExpression *>(callee.get()))
		if (m_Context.HasMacro(name->Name)
			&& m_Context.GetMacro(name->Name).IsCallee)
			return dynamic_pointer_cast<Expression>(
				m_Context.GetMacro(name->Name).Resolve(*this, args));

	return std::make_unique<CallExpression>(where, std::move(callee), args);
}
