#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>

#include "QLang/Context.hpp"

QLang::StatementPtr QLang::Parser::ParseStatement()
{
    if (At(TokenType_CompileDirective))
    {
        ParseCompileDirective();
        return {};
    }

    if (At("{")) return ParseCompound();
    if (At("def")) return ParseDef();
    if (At("if")) return ParseIf();
    if (At("return")) return ParseReturn();
    if (At("while")) return ParseWhile();

    if (At(TokenType_Name) && m_Context.HasMacro(m_State.Tok.Value))
    {
        if (const auto macro = m_Context.GetMacro(Skip().Value); macro.IsCallee)
        {
            std::vector<StatementPtr> args;

            Expect("(");
            while (!NextIfAt(")"))
            {
                args.push_back(ParseStatement());
                if (!At(")"))
                    Expect(",");
            }

            macro.Expand(*this, args);
        }
        else
        {
            macro.Expand(*this);
        }
        return ParseStatement();
    }

    return ParseBinary();
}
