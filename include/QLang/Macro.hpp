#pragma once

#include <QLang/Expression.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Statement.hpp>
#include <string>
#include <vector>

namespace QLang
{
	struct Macro
	{
		StatementPtr Resolve(Parser &parser);
		StatementPtr Resolve(Parser &parser, std::vector<ExpressionPtr> &args);
		StatementPtr Resolve(Parser &parser, std::vector<StatementPtr> &args);

		SourceLocation Where;
		std::string Name;
		std::vector<std::string> Params;
		bool IsCallee;

		std::string Value;
	};
}
