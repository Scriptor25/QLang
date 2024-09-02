#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/SourceLocation.hpp>
#include <iostream>

QLang::Parser::Parser(
	Builder &builder, std::istream &stream, const SourceLocation &where)
	: m_Builder(builder), m_Context(builder.GetContext()), m_Stream(stream),
	  m_Where(where)
{
	Next();
}

QLang::Builder &QLang::Parser::GetBuilder() { return m_Builder; }

bool QLang::Parser::AtEof() const { return m_Token.Type == TokenType_Eof; }

QLang::Token &QLang::Parser::Next() { return m_Token = NextToken(); }

bool QLang::Parser::At(TokenType type) { return m_Token.Type == type; }

bool QLang::Parser::At(const std::string &value)
{
	return m_Token.Value == value;
}

bool QLang::Parser::NextIfAt(TokenType type)
{
	if (At(type))
	{
		Next();
		return true;
	}
	return false;
}

bool QLang::Parser::NextIfAt(const std::string &value)
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
	auto token = m_Token;
	Next();
	return token;
}

QLang::Token QLang::Parser::Expect(TokenType type)
{
	if (At(type)) return Skip();
	std::cerr << "unexpected token" << std::endl;
	return {};
}

QLang::Token QLang::Parser::Expect(const std::string &value)
{
	if (At(value)) return Skip();
	std::cerr << "unexpected token" << std::endl;
	return {};
}
