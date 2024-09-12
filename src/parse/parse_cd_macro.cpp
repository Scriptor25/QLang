#include <string>
#include <vector>
#include <QLang/Context.hpp>
#include <QLang/Parser.hpp>

void QLang::Parser::ParseCDMacro()
{
    Expect("macro");
    UseWhitespace();

    const auto name = Expect(TokenType_Name).Value;
    const bool is_callee = !At(TokenType_Whitespace) && NextIfAt("(");

    IgnoreWhitespace();
    NextIfAt(TokenType_Whitespace);

    std::vector<std::string> params;
    if (is_callee)
        while (!NextIfAt(")"))
        {
            params.push_back(Expect(TokenType_Name).Value);
            if (!At(")")) Expect(",");
        }

    auto& [_where, _name, _params, _is_callee, _value] = m_Context.GetMacro(name);
    _where = m_Token.Where;
    _name = name;
    _params = params;
    _is_callee = is_callee;
    _value = m_Token.Value;

    while (m_C >= 0 && m_C != '\n')
    {
        if (m_C == '\\')
        {
            m_C = Get();
            if (m_C == '\n') NewLine();
            else _value += '\\';
        }

        _value += static_cast<char>(m_C);
        m_C = Get();
    }
    m_C = Get();
    NewLine();
    Next();
}
