#include <QLang/Expression.hpp>
#include <QLang/Macro.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>
#include <sstream>

QLang::StatementPtr QLang::Macro::Resolve(Parser &parser)
{
	std::stringstream stream(Value);
	Parser p(parser.GetBuilder(), stream, Where, parser.GetCallback());
	return p.ParseStatement();
}

QLang::StatementPtr QLang::Macro::Resolve(
	Parser &parser, std::vector<ExpressionPtr> &args)
{
	std::vector<StatementPtr> stmt_args;
	for (auto &arg : args)
		stmt_args.push_back(dyn_cast<Statement>(std::move(arg)));
	return Resolve(parser, stmt_args);
}

QLang::StatementPtr QLang::Macro::Resolve(
	Parser &parser, std::vector<StatementPtr> &args)
{
	std::string value = Value;

	// replace as string
	for (size_t a = 0; a < Params.size(); ++a)
	{
		for (size_t i = 0; i < value.length(); ++i)
		{
			const auto &param = Params[a];
			auto pos = value.find("##" + param + "##", i);
			if (pos == std::string::npos) continue;
			std::stringstream ss;
			ss << '"';
			args[a]->Print(ss);
			ss << '"';
			value.replace(pos, param.length() + 4, ss.str());
			i = pos + param.length() + 4;
		}
	}

	// replace as expression
	for (size_t a = 0; a < Params.size(); ++a)
	{
		for (size_t i = 0; i < value.length(); ++i)
		{
			const auto &param = Params[a];
			auto pos = value.find('#' + param + '#', i);
			if (pos == std::string::npos) continue;
			std::stringstream ss;
			args[a]->Print(ss);
			value.replace(pos, param.length() + 2, ss.str());
			i = pos + param.length() + 2;
		}
	}

	std::stringstream stream(value);
	Parser p(parser.GetBuilder(), stream, Where, parser.GetCallback());
	return p.ParseStatement();
}
