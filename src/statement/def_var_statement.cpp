#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>
#include <ostream>

QLang::DefVarStatement::DefVarStatement(
	const SourceLocation &where, const TypePtr &type, const std::string &name,
	StatementPtr init)
	: DefVarStatement(where, type, name, dyn_cast<Expression>(std::move(init)))
{
}

QLang::DefVarStatement::DefVarStatement(
	const SourceLocation &where, const TypePtr &type, const std::string &name,
	std::vector<StatementPtr> args)
	: DefVarStatement(where, type, name, dyn_cast<Expression>(args))
{
}

QLang::DefVarStatement::DefVarStatement(
	const SourceLocation &where, const TypePtr &type, const std::string &name,
	ExpressionPtr init)
	: Statement(where), Type(type), Name(name), Init(std::move(init))
{
}

QLang::DefVarStatement::DefVarStatement(
	const SourceLocation &where, const TypePtr &type, const std::string &name,
	std::vector<ExpressionPtr> args)
	: Statement(where), Type(type), Name(name), Args(std::move(args))
{
}

std::ostream &QLang::DefVarStatement::Print(std::ostream &stream) const
{
	if (!Init)
	{
		if (Args.empty()) return stream << "def " << Type << ' ' << Name;
		stream << "def " << Type << ' ' << Name << " { ";
		for (size_t i = 0; i < Args.size(); ++i)
		{
			if (i > 0) stream << ", ";
			stream << Args[i];
		}
		return stream << " }";
	}
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
			std::vector<ValuePtr> args;
			std::vector<TypePtr> arg_types;
			for (const auto &arg : Args)
			{
				auto varg = arg->GenIR(builder);
				args.push_back(varg);
				arg_types.push_back(varg->GetType());
			}

			if (auto func = builder.FindConstructor(Type, arg_types))
			{
				init = GenCall(builder, func->AsValue(builder), instance, args);
				if (!init)
				{
					std::cerr << "    at " << Where << std::endl;
					return;
				}
			}
		}
	}

	builder[Name] = instance;
}
