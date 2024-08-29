#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Type.hpp>

QLang::ReferenceTypePtr QLang::ReferenceType::From(const TypePtr &type)
{
	return std::dynamic_pointer_cast<ReferenceType>(type);
}

QLang::ReferenceTypePtr QLang::ReferenceType::Get(const TypePtr &base)
{
	auto &ctx = base->GetCtx();
	auto name = base->GetName() + '&';
	auto &ref = ctx.GetType(name);
	if (!ref) ref = std::make_shared<ReferenceType>(ctx, name, base);
	return ReferenceType::From(ref);
}

QLang::ReferenceType::ReferenceType(
	Context &ctx, const std::string &name, const TypePtr &base)
	: Type(ctx, name, TypeId_Reference, 64), m_Base(base)
{
}

llvm::Type *QLang::ReferenceType::GenIR(Builder &builder) const
{
	return builder.IRBuilder().getPtrTy();
}

QLang::TypePtr QLang::ReferenceType::GetBase() const { return m_Base; }
