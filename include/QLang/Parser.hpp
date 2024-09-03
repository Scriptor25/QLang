#pragma once

#include <QLang/QLang.hpp>
#include <QLang/SourceLocation.hpp>
#include <QLang/Token.hpp>
#include <functional>
#include <istream>

namespace QLang
{
	typedef std::function<void(StatementPtr)> Callback;

	class Parser
	{
	public:
		Parser(Builder &, std::istream &, const SourceLocation &where,
			   Callback callback);

		Builder &GetBuilder();
		Callback GetCallback();

		void Parse();
		StatementPtr ParseStatement();

	private:
		int Get();
		void NewLine();
		void Escape();
		Token NextToken();

		Token &Next();
		bool AtEof() const;
		bool At(TokenType);
		bool At(const std::string &);
		bool NextIfAt(TokenType);
		bool NextIfAt(const std::string &);
		Token Skip();
		Token Expect(TokenType);
		Token Expect(const std::string &);

		TypePtr ParseType();

		void ParseUse();
		void ParseInclude();
		void ParseMacro();

		StatementPtr ParseCompound();
		StatementPtr ParseDef();
		StatementPtr ParseIf();
		StatementPtr ParseReturn();
		StatementPtr ParseWhile();

		StatementPtr ParseBinary();
		StatementPtr ParseBinary(StatementPtr, size_t);

		StatementPtr ParseOperand();
		StatementPtr ParsePrimary();
		StatementPtr ParseCall(StatementPtr);
		StatementPtr ParseIndex(StatementPtr);
		StatementPtr ParseMember(StatementPtr);
		StatementPtr ParseUnary(StatementPtr);

	private:
		Builder &m_Builder;
		Context &m_Context;
		Callback m_Callback;

		std::istream &m_Stream;
		SourceLocation m_Where;
		int m_C = -1;
		Token m_Token;
	};
}
