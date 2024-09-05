#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenNeg(Builder &builder, const ValuePtr &value)
{
	const auto type = value->GetType();

	llvm::Value *result;
	switch (type->GetId())
	{
	case TypeId_Int:
		result = builder.IRBuilder().CreateNeg(value->Get());
		break;
	case TypeId_Float:
		result = builder.IRBuilder().CreateFNeg(value->Get());
		break;

	default: return {};
	}

	return RValue::Create(builder, type, result);
}
