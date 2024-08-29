#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>
#include <ostream>

QLang::DefVarStatement::DefVarStatement(
	const SourceLocation &where, const TypePtr &type, const std::string &name,
	ExpressionPtr init)
	: Statement(where), Type(type), Name(name), Init(std::move(init))
{
}

std::ostream &QLang::DefVarStatement::Print(std::ostream &stream) const
{
	if (!Init) return stream << "def " << Type << ' ' << Name;
	return stream << "def " << Type << ' ' << Name << " = " << Init;
}

void QLang::DefVarStatement::GenIRVoid(Builder &builder) const
{
	if (!builder.IRBuilder().GetInsertBlock())
	{
		std::cerr << "at " << Where << ": global variables are not supported"
				  << std::endl;
		return;
	}

	ValuePtr init;
	if (Init)
	{
		init = Init->GenIR(builder);
		if (!init)
		{
			std::cerr << "    at " << Where << std::endl;
			return;
		}
	}

	LValuePtr value;
	if (auto ref_type = ReferenceType::From(Type))
	{
		auto linit = LValue::From(init);
		if (!linit)
		{
			std::cerr << "at " << Where << ": initializer must be a lvalue here"
					  << std::endl;
			return;
		}
		value = LValue::Create(builder, ref_type->GetBase(), linit->GetPtr());
	}
	else
	{
		if (init)
		{
			init = GenCast(builder, init, Type);
			if (!init)
			{
				std::cerr << "    at " << Where << std::endl;
				return;
			}
		}
		value
			= LValue::Alloca(builder, Type, init ? init->Get() : nullptr, Name);
		builder.DestroyAtEnd().push_back(value);
	}

	builder[Name] = value;
}
