#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::ValuePtr QLang::GenMember(
	Builder &builder, ValuePtr object, bool deref, const std::string &member)
{
	if (deref)
	{
		object = GenDeref(builder, object);
		if (!object) return {};
	}

	if (builder.IsCallee())
	{
		builder.ClearCallee();
		builder.Self() = object;

		if (const auto func = builder.FindFunction(
				member, object->GetType(), builder.GetArgs()))
			return func->AsValue(builder);

		std::cerr << object->GetType() << " does not have a member with name '"
				  << member << "'" << std::endl;
		return {};
	}

	const auto str_type = StructType::From(object->GetType());

	size_t i;
	for (i = 0; i < str_type->GetElementCount(); ++i)
		if (str_type->GetElement(i).Name == member) break;

	if (i >= str_type->GetElementCount())
	{
		std::cerr << object->GetType() << " does not have a member with name '"
				  << member << "'" << std::endl;
		return {};
	}

	const auto lobject = LValue::From(object);
	if (!lobject)
	{
		std::cerr << "object must be a lvalue here" << std::endl;
		return {};
	}

	return GenMember(builder, lobject, i);
}

QLang::LValuePtr QLang::GenMember(
	Builder &builder, const LValuePtr &object, size_t i)
{
	const auto struct_type = StructType::From(object->GetType());
	auto &[type, member, unused] = struct_type->GetElement(i);

	const auto str_ty = object->GetIRType();
	const auto ptr = builder.IRBuilder().CreateStructGEP(
		str_ty, object->GetPtr(), i, member);
	return LValue::Create(builder, type, ptr);
}
