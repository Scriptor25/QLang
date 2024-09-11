#include <utility>
#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Type.hpp>

QLang::ReferenceTypePtr QLang::ReferenceType::From(const TypePtr& type)
{
    return std::dynamic_pointer_cast<ReferenceType>(type);
}

QLang::ReferenceTypePtr QLang::ReferenceType::Get(const TypePtr& base)
{
    auto& ctx = base->GetCtx();
    auto name = base->GetName() + '&';
    auto& ref = ctx.GetType(name);
    if (!ref) ref = std::make_shared<ReferenceType>(ctx, name, base);
    return From(ref);
}

QLang::ReferenceType::ReferenceType(Context& ctx, const std::string& name, TypePtr base)
    : Type(ctx, name, TypeId_Reference, 64), m_Base(std::move(base))
{
}

llvm::Type* QLang::ReferenceType::GenIR(Builder& builder)
{
    if (m_IR)
        return m_IR;

    return m_IR = builder.IRBuilder().getPtrTy();
}

llvm::DIType* QLang::ReferenceType::GenDI(Builder& builder)
{
    if (m_DI)
        return m_DI;

    return m_DI = builder.DIBuilder().createReferenceType(llvm::dwarf::DW_TAG_reference_type, m_Base->GenDI(builder));
}

QLang::TypePtr QLang::ReferenceType::GetBase() const { return m_Base; }
