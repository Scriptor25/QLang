#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>

void QLang::Parser::ParseCDIf()
{
    Expect("#if");

    auto if_bin = ParseBinary();

    if (const auto if_ = dyn_cast<ConstIntExpression>(Collapse(std::move(if_bin))); if_ && if_->Value)
        while (!NextIfAt("#endif"))
            ParseStatement();
    else
        while (!NextIfAt("#endif"))
            Skip();
}
