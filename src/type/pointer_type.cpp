#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Type.hpp>
#include <memory>

QLang::PointerTypePtr QLang::PointerType::From(const TypePtr &type)
{
	return std::dynamic_pointer_cast<PointerType>(type);
}

QLang::PointerTypePtr QLang::PointerType::Get(const TypePtr &base)
{
	auto &ctx = base->GetCtx();
	auto name = base->GetName() + '*';
	auto &ref = ctx.GetType(name);
	if (!ref) ref = std::make_shared<PointerType>(ctx, name, base);
	return PointerType::From(ref);
}

QLang::PointerType::PointerType(
	Context &ctx, const std::string &name, const TypePtr &base)
	: Type(ctx, name, TypeId_Pointer, 64), m_Base(base)
{
}

llvm::PointerType *QLang::PointerType::GenIR(Builder &builder) const
{
	return builder.IRBuilder().getPtrTy();
}

QLang::TypePtr QLang::PointerType::GetBase() const { return m_Base; }
