#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::ValuePtr QLang::GenCast(
	Builder &builder, const ValuePtr &src, const TypePtr &dst_type)
{
	auto src_type = src->GetType();
	if (src_type == dst_type) return src;

	const auto func_name = "operator$" + dst_type->GetName();

	if (const auto self = LValue::From(src))
		if (const auto func = builder.FindFunction(func_name, src_type, {}))
			return GenCall(builder, func->AsValue(builder), self, {});

	if (const auto func = builder.FindFunction(func_name, {}, { src_type }))
		return GenCall(builder, func->AsValue(builder), {}, { src });

	const auto dst_ty = dst_type->GenIR(builder);

	llvm::Value *result = nullptr;
	switch (src_type->GetId())
	{
	case TypeId_Int:
		switch (dst_type->GetId())
		{
		case TypeId_Int:
			result
				= builder.IRBuilder().CreateIntCast(src->Get(), dst_ty, true);
			break;
		case TypeId_Float:
			result = builder.IRBuilder().CreateSIToFP(src->Get(), dst_ty);
			break;
		case TypeId_Pointer:
			result = builder.IRBuilder().CreateIntToPtr(src->Get(), dst_ty);
			break;
		default: break;
		}
		break;

	case TypeId_Float:
		switch (dst_type->GetId())
		{
		case TypeId_Int:
			result = builder.IRBuilder().CreateFPToSI(src->Get(), dst_ty);
			break;
		case TypeId_Float:
			result = builder.IRBuilder().CreateFPCast(src->Get(), dst_ty);
			break;
		default: break;
		}
		break;

	case TypeId_Pointer:
		switch (dst_type->GetId())
		{
		case TypeId_Int:
			result = builder.IRBuilder().CreatePtrToInt(src->Get(), dst_ty);
			break;
		case TypeId_Pointer:
			result = builder.IRBuilder().CreatePointerCast(src->Get(), dst_ty);
			break;
		default: break;
		}
		break;

	case TypeId_Array:
		if (dst_type->IsPointer()) result = LValue::From(src)->GetPtr();
		break;

	default: break;
	}

	if (!result)
	{
		std::cerr << "no cast from " << src_type << " to " << dst_type
				  << std::endl;
		return {};
	}

	return RValue::Create(builder, dst_type, result);
}
