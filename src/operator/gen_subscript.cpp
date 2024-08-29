#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::ValuePtr QLang::GenSubscript(
	Builder &builder, const ValuePtr &array, const ValuePtr &index)
{
	if (auto type = PointerType::From(array->GetType()))
	{
		auto ir_type = type->GetBase()->GenIR(builder);
		auto gep = builder.IRBuilder().CreateGEP(
			ir_type, array->Get(), { index->Get() });
		return LValue::Create(builder, type->GetBase(), gep);
	}

	if (auto type = ArrayType::From(array->GetType()))
	{
		auto larray = LValue::From(array);
		auto ir_type = type->GenIR(builder);
		auto gep = builder.IRBuilder().CreateGEP(
			ir_type, larray->GetPtr(),
			{ builder.IRBuilder().getInt64(0), index->Get() });
		return LValue::Create(builder, type->GetBase(), gep);
	}

	std::cerr << "QLang::GenSubscript" << std::endl;
	return {};
}
