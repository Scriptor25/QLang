#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <memory>

QLang::Value::~Value() = default;

QLang::Builder &QLang::Value::GetBuilder() const { return m_Builder; }

QLang::TypePtr QLang::Value::GetType() const { return m_Type; }

llvm::Type *QLang::Value::GetIRType() const { return m_IRType; }

QLang::Value::Value(Builder &builder, const TypePtr &type)
	: m_Builder(builder), m_Type(type), m_IRType(type->GenIR(builder))
{
}

QLang::RValuePtr QLang::RValue::Create(
	Builder &builder, const TypePtr &type, llvm::Value *value)
{
	return std::make_unique<RValue>(builder, type, value);
}

QLang::RValue::RValue(Builder &builder, const TypePtr &type, llvm::Value *value)
	: Value(builder, type), m_Value(value)
{
}

llvm::Value *QLang::RValue::Get() const { return m_Value; }
