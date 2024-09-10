#include <iostream>
#include <vector>
#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Token.hpp>
#include <QLang/Type.hpp>

QLang::TypePtr QLang::Parser::ParseBaseType()
{
    if (At("struct"))
    {
        auto [Where, Type, Value] = Skip();

        std::string name;
        if (At(TokenType_Name)) name = Skip().Value;
        if (!NextIfAt("{"))
        {
            if (name.empty())
            {
                std::cerr << "at " << Where << ": opaque struct cannot be anonymous" << std::endl;
                return {};
            }
            return StructType::Get(m_Context, name);
        }
        std::vector<StructElement> elements;
        while (!NextIfAt("}"))
        {
            auto& [_type, _name, _init] = elements.emplace_back();
            _type = ParseType();
            _name = Expect(TokenType_Name).Value;
            if (NextIfAt("=")) _init = dyn_cast<Expression>(ParseBinary());
            if (!At("}")) Expect(",");
        }

        return StructType::Get(name, elements);
    }

    if (At(TokenType_Name))
    {
        auto [Where, Type, Value] = Skip();

        if (auto base = Type::Get(m_Context, Value))
            return base;

        std::cerr << "    at " << Where << std::endl;
        return {};
    }

    return {};
}

QLang::TypePtr QLang::Parser::ParseType()
{
    auto base = ParseBaseType();
    if (!base)
        return {};

    while (base)
    {
        if (NextIfAt("*"))
        {
            base = PointerType::Get(base);
            continue;
        }

        if (NextIfAt("&"))
        {
            base = ReferenceType::Get(base);
            continue;
        }

        if (NextIfAt("["))
        {
            const auto length_expr = dyn_cast<ConstIntExpression>(Collapse(ParseBinary()));
            const auto length = length_expr->Value;
            Expect("]");
            base = ArrayType::Get(base, length);
            continue;
        }

        if (NextIfAt("("))
        {
            FnMode mode = FnMode_Func;
            TypePtr self;

            if (NextIfAt("+"))
            {
                mode = FnMode_Ctor;
                self = ParseType();
                Expect(")");
                Expect("(");
            }
            else if (NextIfAt("-"))
            {
                mode = FnMode_Dtor;
                self = ParseType();
                Expect(")");
                Expect("(");
            }
            else if (NextIfAt(":"))
            {
                self = ParseType();
                Expect(")");
                Expect("(");
            }

            std::vector<TypePtr> params;
            bool vararg = false;
            while (!NextIfAt(")"))
            {
                if (NextIfAt("?"))
                {
                    vararg = true;
                    Expect(")");
                    break;
                }

                params.push_back(ParseType());
                NextIfAt(TokenType_Name);
                if (!At(")")) Expect(",");
            }

            base = FunctionType::Get(mode, base, self, params, vararg);
            continue;
        }

        return base;
    }

    return {};
}
