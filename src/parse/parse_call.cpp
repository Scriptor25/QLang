#include <QLang/Context.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>
#include <vector>

QLang::StatementPtr QLang::Parser::ParseCall(StatementPtr callee)
{
	auto where = Expect("(").Where;

	std::vector<StatementPtr> args;
	while (!NextIfAt(")"))
	{
		args.push_back(ParseBinary());
		if (!At(")")) Expect(",");
	}

	if (auto name = dyn_cast<NameExpression>(callee))
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

		callee = std::move(name);
	}

	return std::make_unique<CallExpression>(
		where, std::move(callee), std::move(args));
}
