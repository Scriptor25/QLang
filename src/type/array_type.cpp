#include <QLang/Context.hpp>
#include <QLang/Type.hpp>
#include <memory>
#include <string>

QLang::ArrayTypePtr QLang::ArrayType::Get(const TypePtr &base, uint64_t length)
{
	auto &ctx = base->GetCtx();
	auto name = base->GetName() + '[' + std::to_string(length) + ']';
	auto &ref = ctx.GetType(name);
	if (!ref) ref = std::make_shared<ArrayType>(ctx, name, base, length);
	return std::dynamic_pointer_cast<ArrayType>(ref);
}

QLang::ArrayType::ArrayType(
	Context &ctx, const std::string &name, const TypePtr &base, uint64_t length)
	: Type(ctx, name, base->GetSize() * length), m_Base(base), m_Length(length)
{
}

QLang::TypePtr QLang::ArrayType::GetBase() const { return m_Base; }

uint64_t QLang::ArrayType::GetLength() const { return m_Length; }
