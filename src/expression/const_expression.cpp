#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::ConstCharExpression::ConstCharExpression(
	const SourceLocation &where, char value)
	: Expression(where), Value(value)
{
}

std::ostream &QLang::ConstCharExpression::Print(std::ostream &stream) const
{
	return stream << '\'' << Value << '\'';
}

QLang::ValuePtr QLang::ConstCharExpression::GenIR(Builder &builder) const
{
	return RValue::Create(
		builder, builder.GetInt8Ty(), builder.IRBuilder().getInt8(Value));
}

QLang::ConstFloatExpression::ConstFloatExpression(
	const SourceLocation &where, double value)
	: Expression(where), Value(value)
{
}

std::ostream &QLang::ConstFloatExpression::Print(std::ostream &stream) const
{
	return stream << Value;
}

QLang::ValuePtr QLang::ConstFloatExpression::GenIR(Builder &builder) const
{
	auto value
		= llvm::ConstantFP::get(builder.IRBuilder().getDoubleTy(), Value);
	return RValue::Create(builder, builder.GetFloat64Ty(), value);
}

QLang::ConstIntExpression::ConstIntExpression(
	const SourceLocation &where, uint64_t value)
	: Expression(where), Value(value)
{
}

std::ostream &QLang::ConstIntExpression::Print(std::ostream &stream) const
{
	return stream << Value;
}

QLang::ValuePtr QLang::ConstIntExpression::GenIR(Builder &builder) const
{
	return RValue::Create(
		builder, builder.GetInt64Ty(), builder.IRBuilder().getInt64(Value));
}

QLang::ConstStringExpression::ConstStringExpression(
	const SourceLocation &where, const std::string &value)
	: Expression(where), Value(value)
{
}

std::ostream &QLang::ConstStringExpression::Print(std::ostream &stream) const
{
	return stream << '"' << Value << '"';
}

QLang::ValuePtr QLang::ConstStringExpression::GenIR(Builder &builder) const
{
	return RValue::Create(
		builder, builder.GetInt8PtrTy(),
		builder.IRBuilder().CreateGlobalStringPtr(Value));
}
