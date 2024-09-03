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
		if (!Init)
		{
			auto ir_type = Type->GenIR(builder);
			auto ptr = new llvm::GlobalVariable(
				builder.IRModule(), ir_type, false,
				llvm::GlobalValue::ExternalLinkage, nullptr, Name);
			builder[Name] = LValue::Create(builder, Type, ptr);
			return;
		}

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

	LValuePtr instance;
	if (auto ref_type = ReferenceType::From(Type))
	{
		auto linit = LValue::From(init);
		if (!linit)
		{
			std::cerr << "at " << Where << ": initializer must be a lvalue here"
					  << std::endl;
			return;
		}
		instance
			= LValue::Create(builder, ref_type->GetBase(), linit->GetPtr());
	}
	else
	{
		instance = builder.CreateInstance(Type, Name);
		if (!instance)
		{
			std::cerr << "    at " << Where << std::endl;
			return;
		}

		if (init)
		{
			init = GenCast(builder, init, Type);
			if (!init)
			{
				std::cerr << "    at " << Where << std::endl;
				return;
			}

			instance->Set(init->Get());
		}
		else
		{
			if (auto func = builder.FindConstructor(Type, {}))
			{
				init = GenCall(builder, func->AsValue(builder), instance, {});
				if (!init)
				{
					std::cerr << "    at " << Where << std::endl;
					return;
				}
			}
		}

		builder.DestroyAtEnd().push_back(instance);
	}

	builder[Name] = instance;
}
