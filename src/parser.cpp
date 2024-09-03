#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/SourceLocation.hpp>
#include <QLang/Token.hpp>
#include <iostream>

QLang::Parser::Parser(
	Builder &builder, std::istream &stream, const SourceLocation &where,
	Callback callback)
	: m_Builder(builder), m_Context(builder.GetContext()), m_Stream(stream),
	  m_Where(where), m_Callback(callback)
{
	Next();
}

QLang::Builder &QLang::Parser::GetBuilder() { return m_Builder; }

QLang::Callback QLang::Parser::GetCallback() { return m_Callback; }

void QLang::Parser::Parse()
{
	while (!AtEof())
	{
		auto ptr = ParseStatement();
		if (!ptr) continue;
		m_Callback(std::move(ptr));
	}
}

QLang::Token &QLang::Parser::Next() { return m_Token = NextToken(); }

bool QLang::Parser::AtEof() const { return m_Token.Type == TokenType_Eof; }

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
	std::cerr << "at " << m_Token.Where << ": expected type " << type
			  << ", but is " << m_Token.Type << std::endl;
	return {};
}

QLang::Token QLang::Parser::Expect(const std::string &value)
{
	if (At(value)) return Skip();
	std::cerr << "at " << m_Token.Where << ": expected '" << value
			  << "', but is '" << m_Token.Value << "'" << std::endl;
	return {};
}
