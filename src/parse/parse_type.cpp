#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Token.hpp>
#include <QLang/Type.hpp>
#include <iostream>
#include <vector>

QLang::TypePtr QLang::Parser::ParseType()
{
	TypePtr base;
	if (NextIfAt("struct"))
	{
		std::string name;
		if (At(TokenType_Name)) name = Skip().Value;
		if (!NextIfAt("{")) { base = StructType::Get(m_Context, name); }
		else
		{
			std::vector<StructElement> elements;
			while (!NextIfAt("}"))
			{
				auto &element = elements.emplace_back();
				element.Type = ParseType();
				element.Name = Expect(TokenType_Name).Value;
				if (NextIfAt("="))
					element.Init = dyn_cast<Expression>(ParseBinary());
				if (!At("}")) Expect(",");
			}

			base = StructType::Get(name, elements);
		}
	}
	else if (At(TokenType_Name))
	{
		auto [Where, Type, Value] = Skip();
		base = Type::Get(m_Context, Value);
		if (!base)
		{
			std::cerr << "    at " << Where << std::endl;
			return {};
		}
	}

	while (true)
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
			auto length_expr = dyn_cast<ConstIntExpression>(ParseOperand());
			auto length = length_expr->Value;
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
				if (!At(")")) Expect(",");
			}

			base = FunctionType::Get(mode, base, self, params, vararg);
			continue;
		}

		return base;
	}
}
