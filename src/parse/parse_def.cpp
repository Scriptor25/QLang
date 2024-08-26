#include <QLang/Expression.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Token.hpp>
#include <QLang/Type.hpp>
#include <memory>

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
		name = Expect(TokenType_Name).Value;
	}
	else if (NextIfAt("-"))
	{
		mode = FnMode_Dtor;
		name = Expect(TokenType_Name).Value;
	}
	else
	{
		mode = FnMode_Func;
		type = ParseType();
		name = Expect(TokenType_Name).Value;

		if (NextIfAt(":"))
		{
			self = Type::Get(m_Context, name);
			name = Expect(TokenType_Name).Value;
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
			if (At(TokenType_Name)) param.Name = Skip().Value;
			if (!At(")")) Expect(",");
		}

		if (!type) type = Type::Get(m_Context, "void");

		StatementPtr body;
		if (At("{")) body = ParseCompound();

		return std::make_unique<DefFnStatement>(
			where, mode, type, self, name, params, vararg, std::move(body));
	}

	ExpressionPtr init;
	if (NextIfAt("=")) init = ParseBinary();
	else if (NextIfAt("+"))
	{
		auto callee = std::make_unique<NameExpression>(where, type->GetName());
		init = ParseCall(std::move(callee));
	}

	return std::make_unique<DefVarStatement>(
		where, type, name, std::move(init));
}
