#pragma once

#include <QLang/SourceLocation.hpp>
#include <string>

namespace QLang
{
    enum TokenType
    {
        TokenType_Eof,
        TokenType_Name,
        TokenType_Operator,
        TokenType_BinInt,
        TokenType_OctInt,
        TokenType_DecInt,
        TokenType_HexInt,
        TokenType_Float,
        TokenType_Char,
        TokenType_String,
        TokenType_Other,
        TokenType_Whitespace,
        TokenType_CompileDirective,
    };

    struct Token
    {
        bool operator!() const;

        SourceLocation Where;
        TokenType Type;
        std::string Value;
    };
}
