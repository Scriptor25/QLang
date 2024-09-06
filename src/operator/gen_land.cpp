#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenLAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = builder.IRBuilder().CreateIsNotNull(lhs->Get());
    const auto r = builder.IRBuilder().CreateIsNotNull(rhs->Get());
    const auto value = builder.IRBuilder().CreateAnd(l, r);

    return RValue::Create(builder, builder.GetInt1Ty(), value);
}
