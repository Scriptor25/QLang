#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Token.hpp>
#include <QLang/Type.hpp>
#include <iostream>

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
				if (NextIfAt("=")) element.Init = ParseBinary();
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
			auto length_expr
				= dynamic_pointer_cast<ConstIntExpression>(ParseOperand());
			auto length = length_expr->Value;
			Expect("]");
			base = ArrayType::Get(base, length);
			continue;
		}

		return base;
	}
}
