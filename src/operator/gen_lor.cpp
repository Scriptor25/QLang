#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenLOr(
	Builder &builder, const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto l = builder.IRBuilder().CreateIsNotNull(lhs->Get());
	auto r = builder.IRBuilder().CreateIsNotNull(rhs->Get());
	auto value = builder.IRBuilder().CreateOr(l, r);

	return RValue::Create(builder, builder.GetInt1Ty(), value);
}
