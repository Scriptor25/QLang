#include <QLang/Builder.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <memory>

QLang::Value::~Value() = default;

QLang::Builder &QLang::Value::GetBuilder() const { return m_Builder; }

QLang::TypePtr QLang::Value::GetType() const { return m_Type; }

llvm::Type *QLang::Value::GetIRType() const { return m_IRType; }

QLang::Value::Value(Builder &builder, const TypePtr &type)
	: m_Builder(builder), m_Type(type),
	  m_IRType(type ? type->GenIR(builder) : nullptr)
{
}

QLang::RValuePtr QLang::RValue::From(const ValuePtr &value)
{
	return std::dynamic_pointer_cast<RValue>(value);
}

QLang::RValuePtr QLang::RValue::Create(
	Builder &builder, const TypePtr &type, llvm::Value *value)
{
	return std::make_shared<RValue>(builder, type, value);
}

QLang::RValue::RValue(Builder &builder, const TypePtr &type, llvm::Value *value)
	: Value(builder, type), m_Value(value)
{
}

llvm::Value *QLang::RValue::Get() const { return m_Value; }

QLang::LValuePtr QLang::LValue::From(const ValuePtr &value)
{
	return std::dynamic_pointer_cast<LValue>(value);
}

QLang::LValuePtr QLang::LValue::Create(
	Builder &builder, const TypePtr &type, llvm::Value *ptr)
{
	return std::make_shared<LValue>(builder, type, ptr);
}

QLang::LValuePtr QLang::LValue::Alloca(
	Builder &builder, const TypePtr &type, llvm::Value *value)
{
	auto bb = builder.IRBuilder().GetInsertBlock();
	auto ir_type = type->GenIR(builder);

	builder.IRBuilder().SetInsertPointPastAllocas(bb->getParent());
	auto ptr = builder.IRBuilder().CreateAlloca(ir_type);

	builder.IRBuilder().SetInsertPoint(bb);
	if (value) builder.IRBuilder().CreateStore(value, ptr);

	return std::make_shared<LValue>(builder, type, ptr);
}

QLang::LValue::LValue(Builder &builder, const TypePtr &type, llvm::Value *ptr)
	: Value(builder, type), m_Ptr(ptr)
{
}

llvm::Value *QLang::LValue::Get() const
{
	return GetBuilder().IRBuilder().CreateLoad(GetIRType(), m_Ptr);
}

llvm::Value *QLang::LValue::GetPtr() const { return m_Ptr; }

void QLang::LValue::Set(llvm::Value *value) const
{
	GetBuilder().IRBuilder().CreateStore(value, m_Ptr);
}
