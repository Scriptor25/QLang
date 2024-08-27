#pragma once

#include <QLang/Context.hpp>
#include <QLang/QLang.hpp>
#include <QLang/SourceLocation.hpp>
#include <QLang/Token.hpp>
#include <istream>

namespace QLang
{
	class Parser
	{
	public:
		Parser(Context &, std::istream &, const std::string &filename);

		bool AtEof() const;
		StatementPtr Parse();

	private:
		int Get();
		void NewLine();
		void Escape();
		Token NextToken();

		Token &Next();
		bool At(TokenType);
		bool At(const std::string &);
		bool NextIfAt(TokenType);
		bool NextIfAt(const std::string &);
		Token Skip();
		Token Expect(TokenType);
		Token Expect(const std::string &);

		TypePtr ParseType();

		void ParseUse();

		StatementPtr ParseCompound();
		StatementPtr ParseDef();
		StatementPtr ParseIf();
		StatementPtr ParseReturn();
		StatementPtr ParseWhile();

		ExpressionPtr ParseBinary();
		ExpressionPtr ParseBinary(ExpressionPtr, size_t);

		ExpressionPtr ParseOperand();
		ExpressionPtr ParsePrimary();
		ExpressionPtr ParseCall(ExpressionPtr);
		ExpressionPtr ParseIndex(ExpressionPtr);
		ExpressionPtr ParseMember(ExpressionPtr);
		ExpressionPtr ParseUnary(ExpressionPtr);

	private:
		std::istream &m_Stream;
		SourceLocation m_Where;
		int m_C = -1;
		Token m_Token;
		Context &m_Context;
	};
}
