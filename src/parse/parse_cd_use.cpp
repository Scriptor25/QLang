#include <QLang/Context.hpp>
#include <QLang/Parser.hpp>

void QLang::Parser::ParseCDUse()
{
    Expect("#use");

    const auto name = Expect(TokenType_Name).Value;

    std::vector<std::string> params;
    if (NextIfAt("<"))
    {
        while (!NextIfAt(">"))
        {
            params.push_back(Expect(TokenType_Name).Value);
            if (!At(">"))
                Expect(",");
        }
    }

    Expect("as");

    m_Context.SetTemplateParams(params);
    m_Context.GetType(name) = ParseType();
    m_Context.ClearTemplateParams();
}
