#include <QLang/Parser.hpp>

void QLang::Parser::ParseCompileDirective()
{
    Expect("#");

    if (At("if")) return ParseCDIf();
    if (At("include")) return ParseCDInclude();
    if (At("macro")) return ParseCDMacro();
    if (At("use")) return ParseCDUse();
}
