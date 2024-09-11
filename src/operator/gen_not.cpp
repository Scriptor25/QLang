#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenNot(const SourceLocation& where, Builder& builder, const ValuePtr& value)
{
    const auto type = value->GetType();
    const auto value_ir = value->Get();

    builder.SetLoc(where);

    llvm::Value* result;
    switch (type->GetId())
    {
    case TypeId_Int:
        result = builder.IRBuilder().CreateNot(value_ir);
        break;

    default: return {};
    }

    return RValue::Create(builder, type, result);
}
