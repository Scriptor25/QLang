#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenDec(const SourceLocation& where, Builder& builder, const ValuePtr& value)
{
    const auto type = value->GetType();
    const auto ir_ty = type->GenIR(builder);
    const auto value_ir = value->Get();

    llvm::Constant* one_ir;
    llvm::Value* result;
    switch (type->GetId())
    {
    case TypeId_Int:
        one_ir = llvm::ConstantInt::get(ir_ty, 1, true);
        builder.SetLoc(where);
        result = builder.IRBuilder().CreateSub(value_ir, one_ir);
        break;
    case TypeId_Float:
        one_ir = llvm::ConstantFP::get(ir_ty, 1.0);
        builder.SetLoc(where);
        result = builder.IRBuilder().CreateFSub(value_ir, one_ir);
        break;

    default: return {};
    }

    return RValue::Create(builder, type, result);
}
