#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::CallExpression::CallExpression(
	const SourceLocation &where, ExpressionPtr callee,
	std::vector<ExpressionPtr> &args)
	: Expression(where), Callee(std::move(callee)), Args(std::move(args))
{
}

std::ostream &QLang::CallExpression::Print(std::ostream &stream) const
{
	stream << Callee << '(';
	for (size_t i = 0; i < Args.size(); ++i)
	{
		if (i > 0) stream << ", ";
		stream << Args[i];
	}
	return stream << ')';
}

QLang::ValuePtr QLang::CallExpression::GenIR(Builder &builder) const
{
	builder.SetArgCount(Args.size());

	std::vector<ValuePtr> args;
	for (size_t i = 0; i < Args.size(); ++i)
	{
		auto value = Args[i]->GenIR(builder);
		if (!value)
		{
			std::cerr << "    at " << Where << std::endl;
			return {};
		}

		args.push_back(value);
		builder.GetArg(i) = value->GetType();
	}

	builder.IsCallee() = true;
	auto callee = Callee->GenIR(builder);
	if (!callee)
	{
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

	auto type = FunctionType::FromPtr(callee->GetType());
	if (!type)
	{
		std::cerr << "at " << Where
				  << ": callee must be of type function pointer, but is "
				  << callee->GetType() << std::endl;
		return {};
	}

	LValuePtr self;
	if (type->GetMode() == FnMode_Func && type->GetSelf())
	{
		self = LValue::From(builder.Self());
		if (!self)
		{
			std::cerr << "at " << Where << ": self must be a lvalue here"
					  << std::endl;
			return {};
		}
	}

	auto result = GenCall(builder, callee, self, args);
	if (!result)
	{
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

	if (auto p = dynamic_cast<NameExpression *>(Callee.get());
		p && p->Name == "self")
	{
		auto super = LValue::From(builder["self"]);
		super->Set(result->Get());
		return super;
	}

	return result;
}
