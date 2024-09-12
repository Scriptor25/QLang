#pragma once

#include <QLang/QLang.hpp>
#include <QLang/SourceLocation.hpp>
#include <QLang/Token.hpp>
#include <functional>

namespace QLang
{
    typedef std::function<void(const StatementPtr&)> Callback;

    class Parser
    {
    public:
        Parser(Builder&, std::istream&, SourceLocation where, Callback callback);

        [[nodiscard]] Builder& GetBuilder() const;
        Callback GetCallback();

        void Parse();
        StatementPtr ParseStatement();

    private:
        int Get();
        void NewLine();
        void Escape();
        Token NextToken();

        void UseWhitespace();
        void IgnoreWhitespace();

        Token& Next();
        [[nodiscard]] bool AtEof() const;
        [[nodiscard]] bool At(TokenType) const;
        [[nodiscard]] bool At(const std::string&) const;
        bool NextIfAt(TokenType);
        bool NextIfAt(const std::string&);
        Token Skip();
        Token Expect(TokenType);
        Token Expect(const std::string&);

        TypePtr ParseBaseType();
        TypePtr ParseType();

        void ParseCompileDirective();
        void ParseCDIf();
        void ParseCDInclude();
        void ParseCDMacro();
        void ParseCDUse();

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

        Builder& m_Builder;
        Context& m_Context;
        Callback m_Callback;

        std::istream& m_Stream;
        SourceLocation m_Where;
        int m_C = -1;
        Token m_Token;

        bool m_Whitespace = false;
    };
}
