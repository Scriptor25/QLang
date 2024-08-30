#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>

QLang::ValuePtr QLang::GenInc(Builder &builder, const ValuePtr &value)
{
	auto type = value->GetType();
	auto ir_ty = type->GenIR(builder);

	llvm::Value *result;
	switch (type->GetId())
	{
	case TypeId_Int:
		result = builder.IRBuilder().CreateAdd(
			value->Get(), llvm::ConstantInt::get(ir_ty, 1, true));
		break;
	case TypeId_Float:
		result = builder.IRBuilder().CreateFAdd(
			value->Get(), llvm::ConstantFP::get(ir_ty, 1.0));
		break;

	default: return {};
	}

	return RValue::Create(builder, type, result);
}
