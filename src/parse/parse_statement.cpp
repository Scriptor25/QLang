#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>

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

    return ParseBinary();
}
