#pragma once

#include <QLang/QLang.hpp>
#include <QLang/SourceLocation.hpp>
#include <QLang/Token.hpp>
#include <functional>

namespace QLang
{
    typedef std::function<void(const StatementPtr&)> Callback;

    struct ParserState
    {
        std::shared_ptr<std::istream> Stream;
        int C = -1;
        SourceLocation Where;
        Token Tok;
    };

    class Parser
    {
    public:
        Parser(Builder&, const std::shared_ptr<std::istream>& stream, const std::string& filename, Callback callback);

        [[nodiscard]] Builder& GetBuilder() const;
        Callback GetCallback();

        void Parse();
        StatementPtr ParseStatement();

        void Push(const std::shared_ptr<std::istream>& stream, const SourceLocation& where);
        void Pop();

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

        Builder& m_Builder;
        Context& m_Context;
        Callback m_Callback;

        ParserState m_State;
        std::vector<ParserState> m_StateStack;

        bool m_Whitespace = false;
    };
}
