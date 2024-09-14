#include <string>
#include <vector>
#include <QLang/Context.hpp>
#include <QLang/Parser.hpp>

void QLang::Parser::ParseCDMacro()
{
    Expect("#macro");
    UseWhitespace();
    const auto name = Expect(TokenType_Name).Value;
    IgnoreWhitespace();
    const bool is_callee = !NextIfAt(TokenType_Whitespace) && At("(");

    std::vector<std::string> params;
    if (is_callee)
    {
        Expect("(");
        while (!NextIfAt(")"))
        {
            params.push_back(Expect(TokenType_Name).Value);
            if (!At(")")) Expect(",");
        }
    }

    auto& [where_, name_, params_, is_callee_, value_] = m_Context.GetMacro(name);
    where_ = m_State.Tok.Where;
    name_ = name;
    params_ = params;
    is_callee_ = is_callee;

    value_ = m_State.Tok.Value;
    size_t pos;
    while ((pos = value_.find("#endmacro")) == std::string::npos)
    {
        if (m_State.C == '\n') NewLine();
        value_ += static_cast<char>(m_State.C);
        m_State.C = Get();
    }
    value_ = value_.substr(0, pos);

    Next();
}
