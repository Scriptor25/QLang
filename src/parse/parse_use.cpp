#include <QLang/Context.hpp>
#include <QLang/Parser.hpp>

void QLang::Parser::ParseUse()
{
    Expect("use");
    const auto name = Expect(TokenType_Name).Value;
    Expect("as");
    const auto type = ParseType();

    m_Context.GetType(name) = type;
}
