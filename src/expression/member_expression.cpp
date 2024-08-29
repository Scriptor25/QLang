#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Value.hpp>
#include <iostream>
#include <memory>

QLang::MemberExpression::MemberExpression(
	const SourceLocation &where, ExpressionPtr object, bool dereference,
	const std::string &member)
	: Expression(where), Object(std::move(object)), Dereference(dereference),
	  Member(member)
{
}

std::ostream &QLang::MemberExpression::Print(std::ostream &stream) const
{
	return stream << Object << (Dereference ? '!' : '.') << Member;
}

QLang::ValuePtr QLang::MemberExpression::GenIR(Builder &builder) const
{
	bool is_callee = builder.IsCallee();
	if (is_callee) builder.IsCallee() = false;

	auto object = Object->GenIR(builder);
	if (!object) return {};
	if (Dereference)
	{
		auto ptr_type = PointerType::From(object->GetType());
		object = LValue::Create(builder, ptr_type->GetBase(), object->Get());
	}

	if (is_callee)
	{
		builder.Self() = object;

		auto func = builder.FindFunction(
			Member, object->GetType(), builder.GetArgs());
		if (func)
			return RValue::Create(
				builder, PointerType::Get(func->Type), func->IR);
		return {};
	}

	auto str_type = StructType::From(object->GetType());

	unsigned int i = 0;
	TypePtr type;
	for (; i < str_type->GetElementCount(); ++i)
		if (str_type->GetElement(i).Name == Member)
		{
			type = str_type->GetElement(i).Type;
			break;
		}

	auto lobject = LValue::From(object);
	auto gep = builder.IRBuilder().CreateStructGEP(
		str_type->GenIR(builder), lobject->GetPtr(), i, Member);
	return LValue::Create(builder, type, gep);
}
