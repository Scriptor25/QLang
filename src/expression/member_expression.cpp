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
	if (is_callee) builder.ClearCallee();

	auto object = Object->GenIR(builder);
	if (!object)
	{
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

	if (Dereference)
	{
		auto ptr_type = PointerType::From(object->GetType());
		if (!ptr_type)
		{
			std::cerr << "at " << Where
					  << ": object must be of pointer type here, but is "
					  << object->GetType() << std::endl;
			return {};
		}

		object = LValue::Create(builder, ptr_type->GetBase(), object->Get());
	}

	if (is_callee)
	{
		builder.Self() = object;

		if (auto func = builder.FindFunction(
				Member, object->GetType(), builder.GetArgs()))
			return RValue::Create(
				builder, PointerType::Get(func->Type), func->IR);

		std::cerr << "at " << Where << ": " << object->GetType()
				  << " does not have a member with name '" << Member << "'"
				  << std::endl;
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
	if (i >= str_type->GetElementCount())
	{
		std::cerr << "at " << Where << ": " << object->GetType()
				  << " does not have a member with name '" << Member << "'"
				  << std::endl;
		return {};
	}

	auto lobject = LValue::From(object);
	if (!lobject)
	{
		std::cerr << "at " << Where << ": object must be a lvalue here"
				  << std::endl;
		return {};
	}

	auto gep = builder.IRBuilder().CreateStructGEP(
		str_type->GenIR(builder), lobject->GetPtr(), i, Member);
	return LValue::Create(builder, type, gep);
}
