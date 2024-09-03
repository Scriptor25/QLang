#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>
#include <string>

static std::string unescape(int c)
{
	if (c >= 0x20) return std::string(1, c);
	switch (c)
	{
	case 0x07: return "\\a";
	case 0x08: return "\\b";
	case 0x09: return "\\t";
	case 0x0A: return "\\n";
	case 0x0B: return "\\v";
	case 0x0C: return "\\f";
	case 0x0D: return "\\r";
	default:
		char buf[5];
		sprintf(buf, "\\x%02X", c);
		return buf;
	}
}

static std::string unescape(const std::string &str)
{
	std::string s;
	for (int c : str) s += unescape(c);
	return s;
}

QLang::ConstCharExpression::ConstCharExpression(
	const SourceLocation &where, char value)
	: Expression(where), Value(value)
{
}

std::ostream &QLang::ConstCharExpression::Print(std::ostream &stream) const
{
	return stream << '\'' << unescape(Value) << '\'';
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
	return stream << '"' << unescape(Value) << '"';
}

QLang::ValuePtr QLang::ConstStringExpression::GenIR(Builder &builder) const
{
	return RValue::Create(
		builder, builder.GetInt8PtrTy(),
		builder.IRBuilder().CreateGlobalStringPtr(Value));
}
