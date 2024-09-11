#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenShl(const SourceLocation& where, Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    const auto lhs_ir = lhs->Get();
    const auto rhs_ir = rhs->Get();

    builder.SetLoc(where);

    llvm::Value* value;
    switch (type->GetId())
    {
    case TypeId_Int:
        value = builder.IRBuilder().CreateShl(lhs_ir, rhs_ir);
        break;

    default: return {};
    }

    return RValue::Create(builder, type, value);
}
