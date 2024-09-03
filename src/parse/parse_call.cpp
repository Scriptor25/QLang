#include <QLang/Context.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>
#include <vector>

QLang::StatementPtr QLang::Parser::ParseCall(ExpressionPtr callee)
{
	auto where = Expect("(").Where;

	std::vector<ExpressionPtr> args;
	while (!NextIfAt(")"))
	{
		args.push_back(dynamic_pointer_cast<Expression>(ParseBinary()));
		if (!At(")")) Expect(",");
	}

	if (auto name = dynamic_cast<NameExpression *>(callee.get()))
	{
		if (m_Context.HasMacro(name->Name))
		{
			auto &macro = m_Context.GetMacro(name->Name);
			if (macro.IsCallee)
			{
				auto statement
					= m_Context.GetMacro(name->Name).Resolve(*this, args);
				return statement;
			}
		}
	}

	return std::make_unique<CallExpression>(where, std::move(callee), args);
}
