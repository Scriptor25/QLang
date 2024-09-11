#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenLOr(const SourceLocation& where, Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto lhs_ir = builder.IRBuilder().CreateIsNotNull(lhs->Get());
    const auto rhs_ir = builder.IRBuilder().CreateIsNotNull(rhs->Get());
    builder.SetLoc(where);
    const auto value = builder.IRBuilder().CreateOr(lhs_ir, rhs_ir);

    return RValue::Create(builder, builder.GetInt1Ty(), value);
}
