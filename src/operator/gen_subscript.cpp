#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::ValuePtr QLang::GenSubscript(
	Builder &builder, const ValuePtr &array, const ValuePtr &index)
{
	if (const auto type = PointerType::From(array->GetType()))
	{
		const auto ir_type = type->GetBase()->GenIR(builder);
		const auto gep = builder.IRBuilder().CreateGEP(
			ir_type, array->Get(), { index->Get() });
		return LValue::Create(builder, type->GetBase(), gep);
	}

	if (const auto type = ArrayType::From(array->GetType()))
	{
		const auto l_array = LValue::From(array);
		const auto ir_type = type->GenIR(builder);
		const auto gep = builder.IRBuilder().CreateGEP(
			ir_type, l_array->GetPtr(),
			{ builder.IRBuilder().getInt64(0), index->Get() });
		return LValue::Create(builder, type->GetBase(), gep);
	}

	std::cerr
		<< "subscript operand must be either an array or a pointer, but is "
		<< array->GetType() << std::endl;
	return {};
}
