#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
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

		if (auto func = builder.FindFunction(
				member, object->GetType(), builder.GetArgs()))
			return func->AsValue(builder);

		std::cerr << object->GetType() << " does not have a member with name '"
				  << member << "'" << std::endl;
		return {};
	}

	auto str_type = StructType::From(object->GetType());

	TypePtr type;
	size_t i;
	for (i = 0; i < str_type->GetElementCount(); ++i)
		if (str_type->GetElement(i).Name == member)
		{
			type = str_type->GetElement(i).Type;
			break;
		}

	if (!type)
	{
		std::cerr << object->GetType() << " does not have a member with name '"
				  << member << "'" << std::endl;
		return {};
	}

	auto lobject = LValue::From(object);
	if (!lobject)
	{
		std::cerr << "object must be a lvalue here" << std::endl;
		return {};
	}

	auto gep = builder.IRBuilder().CreateStructGEP(
		str_type->GenIR(builder), lobject->GetPtr(), i, member);
	return LValue::Create(builder, type, gep);
}
