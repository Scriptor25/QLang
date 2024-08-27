#include <QLang/Context.hpp>
#include <QLang/Type.hpp>

QLang::ReferenceTypePtr QLang::ReferenceType::Get(const TypePtr &base)
{
	auto &ctx = base->GetCtx();
	auto name = base->GetName() + '&';
	auto &ref = ctx.GetType(name);
	if (!ref) ref = std::make_shared<ReferenceType>(ctx, name, base);
	return std::dynamic_pointer_cast<ReferenceType>(ref);
}

QLang::ReferenceType::ReferenceType(
	Context &ctx, const std::string &name, const TypePtr &base)
	: Type(ctx, name, base->GetSize())
{
}

QLang::TypePtr QLang::ReferenceType::GetBase() const { return m_Base; }
