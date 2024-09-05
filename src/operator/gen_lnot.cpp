#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenLNot(Builder &builder, const ValuePtr &value)
{
	const auto result = builder.IRBuilder().CreateIsNull(value->Get());
	return RValue::Create(builder, builder.GetInt1Ty(), result);
}
