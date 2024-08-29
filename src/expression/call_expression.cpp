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
		if (!value) return {};
		args.push_back(value);
		builder.GetArg(i) = value->GetType();
	}

	builder.IsCallee() = true;
	auto callee = Callee->GenIR(builder);
	if (!callee) return {};

	auto type = FunctionType::FromPtr(callee->GetType());

	LValuePtr self;
	if (type->GetMode() == FnMode_Func && type->GetSelf())
		self = LValue::From(builder.Self());

	auto result = GenCall(builder, callee, self, args);

	if (auto p = dynamic_cast<NameExpression *>(Callee.get());
		p && p->Name == "self")
	{
		auto super = LValue::From(builder["self"]);
		super->Set(result->Get());
		return super;
	}

	return result;
}
