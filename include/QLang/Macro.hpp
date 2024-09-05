#pragma once

#include <QLang/QLang.hpp>
#include <QLang/Statement.hpp>
#include <string>
#include <vector>

namespace QLang
{
	struct Macro
	{
		StatementPtr Resolve(Parser &parser) const;
		StatementPtr Resolve(
			Parser &parser, std::vector<ExpressionPtr> &args) const;
		StatementPtr Resolve(
			Parser &parser, std::vector<StatementPtr> &args) const;

		SourceLocation Where;
		std::string Name;
		std::vector<std::string> Params;
		bool IsCallee;

		std::string Value;
	};
}
