#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenDiv(
	Builder &builder, const ValuePtr &lhs, const ValuePtr &rhs)
{
	const auto type = lhs->GetType();

	llvm::Value *value;
	switch (type->GetId())
	{
	case TypeId_Int:
		value = builder.IRBuilder().CreateSDiv(lhs->Get(), rhs->Get());
		break;
	case TypeId_Float:
		value = builder.IRBuilder().CreateFDiv(lhs->Get(), rhs->Get());
		break;

	default: return {};
	}

	return RValue::Create(builder, type, value);
}
