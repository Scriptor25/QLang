#include <iostream>
#include <stdexcept>
#include <utility>
#include <QLang/Builder.hpp>
#include <QLang/Parser.hpp>
#include <QLang/SourceLocation.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Token.hpp>

QLang::Parser::Parser(
    Builder& builder,
    const std::shared_ptr<std::istream>& stream,
    const std::string& filename,
    Callback callback)
    : m_Builder(builder),
      m_Context(builder.GetContext()),
      m_Callback(std::move(callback)),
      m_State({stream, -1, {filename}})
{
    m_State.C = Get();
    Next();
}

QLang::Builder& QLang::Parser::GetBuilder() const { return m_Builder; }

QLang::Callback QLang::Parser::GetCallback() { return m_Callback; }

void QLang::Parser::Parse()
{
    while (!AtEof())
    {
        auto ptr = ParseStatement();
        if (!ptr) continue;
        m_Callback(ptr);
    }
}

void QLang::Parser::Push(const std::shared_ptr<std::istream>& stream, const SourceLocation& where)
{
    m_StateStack.push_back(m_State);
    m_State.Stream = stream;
    m_State.C = stream->get();
    m_State.Where = where;
    m_State.Tok = NextToken();
}

void QLang::Parser::Pop()
{
    if (m_StateStack.empty())
        return;
    m_State = m_StateStack.back();
    m_StateStack.pop_back();
}

QLang::Token& QLang::Parser::Next() { return m_State.Tok = NextToken(); }

bool QLang::Parser::AtEof() const { return m_State.Tok.Type == TokenType_Eof; }

bool QLang::Parser::At(const TokenType type) const
{
    return m_State.Tok.Type == type;
}

bool QLang::Parser::At(const std::string& value) const
{
    return m_State.Tok.Value == value;
}

bool QLang::Parser::NextIfAt(const TokenType type)
{
    if (At(type))
    {
        Next();
        return true;
    }
    return false;
}

bool QLang::Parser::NextIfAt(const std::string& value)
{
    if (At(value))
    {
        Next();
        return true;
    }
    return false;
}

QLang::Token QLang::Parser::Skip()
{
    auto token = m_State.Tok;
    Next();
    return token;
}

QLang::Token QLang::Parser::Expect(TokenType type)
{
    if (At(type)) return Skip();
    std::cerr
        << "at "
        << m_State.Tok.Where
        << ": expected type "
        << type
        << ", but is "
        << m_State.Tok.Type
        << std::endl;
    throw std::runtime_error("QLang::Parser::Expect");
}

QLang::Token QLang::Parser::Expect(const std::string& value)
{
    if (At(value)) return Skip();
    std::cerr
        << "at "
        << m_State.Tok.Where
        << ": expected '"
        << value
        << "', but is '"
        << m_State.Tok.Value
        << "'"
        << std::endl;
    throw std::runtime_error("QLang::Parser::Expect");
}
