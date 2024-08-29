#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenSub(
	Builder &builder, const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto type = lhs->GetType();

	llvm::Value *value;
	switch (type->GetId())
	{
	case TypeId_Int:
		value = builder.IRBuilder().CreateSub(lhs->Get(), rhs->Get());
		break;
	case TypeId_Float:
		value = builder.IRBuilder().CreateFSub(lhs->Get(), rhs->Get());
		break;

	default: return {};
	}

	return RValue::Create(builder, type, value);
}
