#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenOr(
	Builder &builder, const ValuePtr &lhs, const ValuePtr &rhs)
{
	const auto type = lhs->GetType();

	llvm::Value *value;
	switch (type->GetId())
	{
	case TypeId_Int:
		value = builder.IRBuilder().CreateOr(lhs->Get(), rhs->Get());
		break;

	default: return {};
	}

	return RValue::Create(builder, type, value);
}
