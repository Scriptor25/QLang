#include <QLang/Function.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::RValuePtr QLang::Function::AsValue(Builder& builder) const
{
	return RValue::Create(builder, PointerType::Get(Type), IR);
}
