#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>

void QLang::Parser::ParseCDIf()
{
    Expect("if");
    if (const auto if_ = dyn_cast<ConstIntExpression>(Collapse(ParseBinary())); if_ && if_->Value)
        while (!NextIfAt("endif"))
            ParseStatement();
    else
        while (!NextIfAt("endif"))
            Skip();
}
