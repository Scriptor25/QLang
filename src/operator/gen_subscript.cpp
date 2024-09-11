#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::ValuePtr QLang::GenSubscript(
    const SourceLocation& where,
    Builder& builder,
    const ValuePtr& array,
    const ValuePtr& index)
{
    if (const auto type = PointerType::From(array->GetType()))
    {
        const auto array_ir = array->Get();
        const auto index_ir = index->Get();

        const auto ir_type = type->GetBase()->GenIR(builder);
        builder.SetLoc(where);
        const auto gep = builder.IRBuilder().CreateGEP(ir_type, array_ir, {index_ir});
        return LValue::Create(builder, type->GetBase(), gep);
    }

    if (const auto type = ArrayType::From(array->GetType()))
    {
        const auto zero_ir = builder.IRBuilder().getInt64(0);
        const auto index_ir = index->Get();

        const auto l_array = LValue::From(array);
        const auto ir_type = type->GenIR(builder);
        builder.SetLoc(where);
        const auto gep = builder.IRBuilder().CreateGEP(ir_type, l_array->GetPtr(), {zero_ir, index_ir});
        return LValue::Create(builder, type->GetBase(), gep);
    }

    std::cerr
        << "at "
        << where
        << ": subscript operand must be either an array or a pointer, but is "
        << array->GetType()
        << std::endl;
    return {};
}
