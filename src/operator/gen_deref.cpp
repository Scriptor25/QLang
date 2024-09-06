#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenDeref(Builder& builder, const ValuePtr& value)
{
    if (const auto type = PointerType::From(value->GetType()))
        return LValue::Create(builder, type->GetBase(), value->Get());
    return {};
}
