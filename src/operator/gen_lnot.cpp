#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenLNot(const SourceLocation& where, Builder& builder, const ValuePtr& value)
{
    const auto value_ir = value->Get();
    builder.SetLoc(where);
    const auto result = builder.IRBuilder().CreateIsNull(value_ir);
    return RValue::Create(builder, builder.GetInt1Ty(), result);
}
