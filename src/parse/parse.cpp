#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>

QLang::StatementPtr QLang::Parser::Parse()
{
	if (At("use"))
	{
		ParseUse();
		return {};
	}
	if (At("include"))
	{
		ParseInclude();
		return {};
	}

	if (At("{")) return ParseCompound();
	if (At("def")) return ParseDef();
	if (At("if")) return ParseIf();
	if (At("return")) return ParseReturn();
	if (At("while")) return ParseWhile();

	return ParseBinary();
}
