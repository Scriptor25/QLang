#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/SourceLocation.hpp>
#include <QLang/Token.hpp>
#include <iostream>
#include <stdexcept>
#include <utility>

QLang::Parser::Parser(
	Builder &builder, std::istream &stream, SourceLocation where,
	Callback callback)
	: m_Builder(builder), m_Context(builder.GetContext()),
	  m_Callback(std::move(callback)), m_Stream(stream),
	  m_Where(std::move(where))
{
	Next();
}

QLang::Builder &QLang::Parser::GetBuilder() const { return m_Builder; }

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

QLang::Token &QLang::Parser::Next() { return m_Token = NextToken(); }

bool QLang::Parser::AtEof() const { return m_Token.Type == TokenType_Eof; }

bool QLang::Parser::At(const TokenType type) const
{
	return m_Token.Type == type;
}

bool QLang::Parser::At(const std::string &value) const
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
	std::cerr << "at " << m_Token.Where << ": expected type " << type
			  << ", but is " << m_Token.Type << std::endl;
	throw std::runtime_error("QLang::Parser::Expect");
}

QLang::Token QLang::Parser::Expect(const std::string &value)
{
	if (At(value)) return Skip();
	std::cerr << "at " << m_Token.Where << ": expected '" << value
			  << "', but is '" << m_Token.Value << "'" << std::endl;
	throw std::runtime_error("QLang::Parser::Expect");
}
