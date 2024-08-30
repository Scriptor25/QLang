#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenRef(Builder &builder, const ValuePtr &value)
{
	if (auto lvalue = LValue::From(value))
		return RValue::Create(
			builder, PointerType::Get(lvalue->GetType()), lvalue->GetPtr());
	return {};
}
