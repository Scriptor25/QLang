#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::ValuePtr QLang::GenNot(Builder &builder, const ValuePtr &value)
{
	const auto type = value->GetType();

	llvm::Value *result;
	switch (type->GetId())
	{
	case TypeId_Int:
		result = builder.IRBuilder().CreateNot(value->Get());
		break;

	default: return {};
	}

	return RValue::Create(builder, type, result);
}
