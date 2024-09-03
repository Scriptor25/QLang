#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Token.hpp>
#include <QLang/Type.hpp>
#include <iostream>
#include <memory>
#include <ostream>

QLang::StatementPtr QLang::Parser::ParseDef()
{
	auto where = Expect("def").Where;

	FnMode mode;
	TypePtr type;
	TypePtr self;
	std::string name;

	if (NextIfAt("+"))
	{
		mode = FnMode_Ctor;
		type = Type::Get(m_Context, "void");
		self = Type::Get(m_Context, Expect(TokenType_Name).Value);
		if (!self)
		{
			std::cerr << "    at " << where << std::endl;
			return {};
		}
		name = self->GetName();
	}
	else if (NextIfAt("-"))
	{
		mode = FnMode_Dtor;
		type = Type::Get(m_Context, "void");
		self = Type::Get(m_Context, Expect(TokenType_Name).Value);
		if (!self)
		{
			std::cerr << "    at " << where << std::endl;
			return {};
		}
		name = self->GetName();
	}
	else
	{
		mode = FnMode_Func;
		type = ParseType();
		if (!type)
		{
			std::cerr << "    at " << where << std::endl;
			return {};
		}
		name = Expect(TokenType_Name).Value;

		if (NextIfAt(":"))
		{
			self = Type::Get(m_Context, name);
			if (!self)
			{
				std::cerr << "    at " << where << std::endl;
				return {};
			}
			name = Expect(TokenType_Name).Value;
		}

		if (name == "operator")
		{
			if (NextIfAt("$")) { name += "$" + type->GetName(); }
			else
			{
				do name += Skip().Value;
				while (!At("("));
			}
		}
	}

	if (NextIfAt("("))
	{
		std::vector<Param> params;
		bool vararg = false;
		while (!NextIfAt(")"))
		{
			if (NextIfAt("?"))
			{
				vararg = true;
				Expect(")");
				break;
			}

			auto &param = params.emplace_back();

			param.Type = ParseType();
			if (!param.Type)
			{
				std::cerr << "    at " << where << std::endl;
				return {};
			}

			if (At(TokenType_Name)) param.Name = Skip().Value;
			if (!At(")")) Expect(",");
		}

		StatementPtr body;
		if (At("{")) body = ParseCompound();

		return std::make_unique<DefFnStatement>(
			where, mode, type, self, name, params, vararg, std::move(body));
	}

	StatementPtr init;
	if (NextIfAt("=")) init = ParseBinary();
	else if (At("+"))
	{
		auto w = Skip().Where;
		auto callee = std::make_unique<NameExpression>(w, type->GetName());
		init = ParseCall(std::move(callee));
	}

	return std::make_unique<DefVarStatement>(
		where, type, name, std::move(init));
}
