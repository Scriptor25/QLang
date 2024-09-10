#include <memory>
#include <utility>
#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Type.hpp>

QLang::PointerTypePtr QLang::PointerType::From(const TypePtr& type)
{
    return std::dynamic_pointer_cast<PointerType>(type);
}

QLang::PointerTypePtr QLang::PointerType::Get(const TypePtr& base)
{
    auto& ctx = base->GetCtx();
    auto name = base->GetName() + '*';
    auto& ref = ctx.GetType(name);
    if (!ref) ref = std::make_shared<PointerType>(ctx, name, base);
    return From(ref);
}

QLang::PointerType::PointerType(Context& ctx, const std::string& name, TypePtr base)
    : Type(ctx, name, TypeId_Pointer, 64), m_Base(std::move(base))
{
}

llvm::PointerType* QLang::PointerType::GenIR(Builder& builder) const
{
    return builder.IRBuilder().getPtrTy();
}

llvm::DIType* QLang::PointerType::GenDI(Builder& builder) const
{
    return builder.DIBuilder().createPointerType(m_Base->GenDI(builder), 64);
}

QLang::TypePtr QLang::PointerType::GetBase() const { return m_Base; }
