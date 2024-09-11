#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::ValuePtr QLang::GenMember(
    const SourceLocation& where,
    Builder& builder,
    ValuePtr object,
    const bool deref,
    const std::string& member)
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

        if (const auto func = builder.FindFunction(member, object->GetType(), builder.GetArgs()))
            return func->AsValue(builder);

        std::cerr
            << "at "
            << where
            << ": "
            << object->GetType()
            << " does not have a member with name '"
            << member
            << "'"
            << std::endl;
        return {};
    }

    const auto str_type = StructType::From(object->GetType());

    size_t i;
    for (i = 0; i < str_type->GetElementCount(); ++i)
        if (str_type->GetElement(i).Name == member) break;

    if (i >= str_type->GetElementCount())
    {
        std::cerr
            << "at "
            << where
            << ": "
            << object->GetType()
            << " does not have a member with name '"
            << member
            << "'"
            << std::endl;
        return {};
    }

    const auto l_object = LValue::From(object);
    if (!l_object)
    {
        std::cerr << "at " << where << ": object must be a lvalue here" << std::endl;
        return {};
    }

    return GenMember(where, builder, l_object, i);
}

QLang::LValuePtr QLang::GenMember(
    const SourceLocation& where,
    Builder& builder,
    const LValuePtr& object,
    const size_t i)
{
    const auto struct_type = StructType::From(object->GetType());
    auto& [type, member, unused] = struct_type->GetElement(i);

    const auto str_ty = object->GetIRType();
    builder.SetLoc(where);
    const auto ptr = builder.IRBuilder().CreateStructGEP(str_ty, object->GetPtr(), i, member);
    return LValue::Create(builder, type, ptr);
}
