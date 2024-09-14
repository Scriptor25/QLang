#include <QLang/Parser.hpp>
#include <QLang/Token.hpp>
#include <istream>

static int is_oct_digit(const int c) { return 0x30 <= c && c <= 0x37; }

static int is_operator(const int c)
{
    return c == '+'
        || c == '-'
        || c == '*'
        || c == '/'
        || c == '%'
        || c == '&'
        || c == '|'
        || c == '^'
        || c == '='
        || c == '<'
        || c == '>'
        || c == '!'
        || c == '~'
        || c == '?';
}

static int is_compound_operator(const int c)
{
    return c == '+'
        || c == '-'
        || c == '&'
        || c == '|'
        || c == '^'
        || c == '='
        || c == '<'
        || c == '>';
}

int QLang::Parser::Get()
{
    ++m_Where.Column;
    return m_Stream.get();
}

void QLang::Parser::NewLine()
{
    m_Where.Column = 0;
    ++m_Where.Row;
}

void QLang::Parser::Escape()
{
    if (m_C != '\\') return;

    m_C = Get();
    switch (m_C)
    {
    case 'a':
        m_C = 0x07;
        break;
    case 'b':
        m_C = 0x08;
        break;
    case 't':
        m_C = 0x09;
        break;
    case 'n':
        m_C = 0x0A;
        break;
    case 'v':
        m_C = 0x0B;
        break;
    case 'f':
        m_C = 0x0C;
        break;
    case 'r':
        m_C = 0x0D;
        break;
    case 'x':
        {
            m_C = Get();
            std::string value;
            value += static_cast<char>(m_C);
            m_C = Get();
            value += static_cast<char>(m_C);
            m_C = std::stoi(value, nullptr, 16);
        }
        break;
    default:
        if (is_oct_digit(m_C))
        {
            std::string value;
            value += static_cast<char>(m_C);
            m_C = Get();
            value += static_cast<char>(m_C);
            m_C = Get();
            value += static_cast<char>(m_C);
            m_C = std::stoi(value, nullptr, 8);
        }
        break;
    }
}

QLang::Token QLang::Parser::NextToken()
{
    enum State
    {
        State_Normal,
        State_Comment,
        State_SLComment,
        State_MLComment,
        State_Name,
        State_Radix,
        State_Bin,
        State_Oct,
        State_Dec,
        State_Hex,
        State_Char,
        State_String,
        State_Operator,
        State_Whitespace,
        State_CompileDirective,
    };

    if (m_C < 0) m_C = Get();

    auto state = State_Normal;
    bool is_float;
    std::string value;
    SourceLocation where;

    while (m_C >= 0 || state != State_Normal)
    {
        switch (state)
        {
        case State_Normal:
            switch (m_C)
            {
            case ';': state = State_Comment;
                break;

            case '\\':
                m_C = Get();
                if (m_C == '\n') NewLine();
                break;

            case '"':
                where = m_Where;
                state = State_String;
                break;

            case '\'':
                where = m_Where;
                state = State_Char;
                break;

            case '0':
                where = m_Where;
                value += static_cast<char>(m_C);
                state = State_Radix;
                break;

            case '\r':
                m_Where.Column = 0;
                break;

            case '\n':
                NewLine();
                break;

            case '#':
                where = m_Where;
                value += static_cast<char>(m_C);
                state = State_CompileDirective;
                break;

            default:
                if (m_C <= 0x20)
                {
                    if (!m_Whitespace)
                        break;

                    where = m_Where;
                    state = State_Whitespace;
                    value += static_cast<char>(m_C);
                    break;
                }

                if (is_operator(m_C))
                {
                    where = m_Where;
                    state = State_Operator;
                    value += static_cast<char>(m_C);
                    break;
                }

                if (isdigit(m_C))
                {
                    where = m_Where;
                    state = State_Dec;
                    is_float = false;
                    value += static_cast<char>(m_C);
                    break;
                }

                if (isalnum(m_C) || m_C == '_')
                {
                    where = m_Where;
                    state = State_Name;
                    value += static_cast<char>(m_C);
                    break;
                }

                where = m_Where;
                value += static_cast<char>(m_C);
                m_C = Get();

                if (value[0] == '.' && isdigit(m_C))
                {
                    state = State_Dec;
                    is_float = true;
                    value += static_cast<char>(m_C);
                    break;
                }

                return {where, TokenType_Other, value};
            }
            break;

        case State_Whitespace:
            if (m_C <= 0x20)
            {
                value += static_cast<char>(m_C);
                if (m_C == '\n')
                    NewLine();
                break;
            }
            return {where, TokenType_Whitespace, value};

        case State_Comment:
            state = m_C == ';' ? State_SLComment : State_MLComment;
            break;

        case State_SLComment:
            if (m_C == '\n' || m_C < 0)
            {
                NewLine();
                state = State_Normal;
            }
            break;

        case State_MLComment:
            if (m_C == ';' || m_C < 0)
                state = State_Normal;
            else if (m_C == '\n')
                NewLine();
            break;

        case State_String:
            if (m_C != '"' && m_C >= 0)
            {
                if (m_C == '\\') Escape();
                value += static_cast<char>(m_C);
                break;
            }
            m_C = Get();
            return {where, TokenType_String, value};

        case State_Char:
            if (m_C != '\'' && m_C >= 0)
            {
                if (m_C == '\\') Escape();
                value += static_cast<char>(m_C);
                break;
            }
            m_C = Get();
            return {where, TokenType_Char, value};

        case State_Radix:
            if (m_C == 'b' || m_C == 'B')
            {
                value += static_cast<char>(m_C);
                state = State_Bin;
                break;
            }
            if (m_C == 'x' || m_C == 'X')
            {
                value += static_cast<char>(m_C);
                state = State_Hex;
                break;
            }
            if (m_C == '.')
            {
                is_float = true;
                value += static_cast<char>(m_C);
                state = State_Dec;
                break;
            }
            if (is_oct_digit(m_C))
            {
                value += static_cast<char>(m_C);
                state = State_Oct;
                break;
            }
            return {where, TokenType_DecInt, "0"};

        case State_Bin:
            if (m_C == '0' || m_C == '1' || m_C == 'u')
            {
                value += static_cast<char>(m_C);
                break;
            }
            return {where, TokenType_BinInt, value};

        case State_Oct:
            if (is_oct_digit(m_C) || m_C == 'u')
            {
                value += static_cast<char>(m_C);
                break;
            }
            return {where, TokenType_OctInt, value};

        case State_Dec:
            if (m_C == '.')
            {
                is_float = true;
                value += static_cast<char>(m_C);
                break;
            }
            if (isdigit(m_C) || m_C == 'u')
            {
                value += static_cast<char>(m_C);
                break;
            }
            return {where, is_float ? TokenType_Float : TokenType_DecInt, value};

        case State_Hex:
            if (isxdigit(m_C) || m_C == 'u')
            {
                value += static_cast<char>(m_C);
                break;
            }
            return {where, TokenType_HexInt, value};

        case State_Name:
            if (isalnum(m_C) || m_C == '_')
            {
                value += static_cast<char>(m_C);
                break;
            }
            return {where, TokenType_Name, value};

        case State_Operator:
            if (is_compound_operator(m_C))
            {
                value += static_cast<char>(m_C);
                break;
            }
            return {where, TokenType_Operator, value};

        case State_CompileDirective:
            if (isalnum(m_C) || m_C == '_')
            {
                value += static_cast<char>(m_C);
                break;
            }
            return {where, TokenType_CompileDirective, value};
        }

        m_C = Get();
    }

    return {m_Where, TokenType_Eof, ""};
}

void QLang::Parser::UseWhitespace()
{
    m_Whitespace = true;
}

void QLang::Parser::IgnoreWhitespace()
{
    m_Whitespace = false;
}
