#include <QLang/Expression.hpp>
#include <QLang/Macro.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>
#include <sstream>

void QLang::Macro::Expand(Parser& parser) const
{
    parser.Push(std::make_shared<std::stringstream>(Value), Where);
}

void QLang::Macro::Expand(Parser& parser, std::vector<ExpressionPtr>& args) const
{
    std::vector<StatementPtr> stmt_args;
    stmt_args.reserve(args.size());
    for (auto& arg : args)
        stmt_args.push_back(dyn_cast<Statement>(std::move(arg)));
    Expand(parser, stmt_args);
}

void QLang::Macro::Expand(Parser& parser, std::vector<StatementPtr>& args) const
{
    std::string value = Value;

    // replace as string
    for (size_t a = 0; a < Params.size(); ++a)
    {
        for (size_t i = 0; i < value.length(); ++i)
        {
            const auto& param = Params[a];
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
            const auto& param = Params[a];
            auto pos = value.find('#' + param + '#', i);
            if (pos == std::string::npos) continue;
            std::stringstream ss;
            args[a]->Print(ss);
            value.replace(pos, param.length() + 2, ss.str());
            i = pos + param.length() + 2;
        }
    }

    parser.Push(std::make_shared<std::stringstream>(value), Where);
}
