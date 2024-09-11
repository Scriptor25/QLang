#include <memory>
#include <string>
#include <llvm/IR/DerivedTypes.h>
#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Type.hpp>

QLang::ArrayTypePtr QLang::ArrayType::From(const TypePtr& type)
{
    return std::dynamic_pointer_cast<ArrayType>(type);
}

QLang::ArrayTypePtr QLang::ArrayType::Get(const TypePtr& base, uint64_t length)
{
    auto& ctx = base->GetCtx();
    auto name = base->GetName() + '[' + std::to_string(length) + ']';
    auto& ref = ctx.GetType(name);
    if (!ref) ref = std::make_shared<ArrayType>(ctx, name, base, length);
    return From(ref);
}

QLang::ArrayType::ArrayType(Context& ctx,
                            const std::string& name,
                            const TypePtr& base,
                            const uint64_t length)
    : Type(ctx, name, TypeId_Array, base->GetSize() * length), m_Base(base), m_Length(length)
{
}

llvm::ArrayType* QLang::ArrayType::GenIR(Builder& builder)
{
    if (m_IR)
        return m_IR;

    return m_IR = llvm::ArrayType::get(m_Base->GenIR(builder), m_Length);
}

llvm::DIType* QLang::ArrayType::GenDI(Builder& builder)
{
    if (m_DI)
        return m_DI;

    return m_DI = builder.DIBuilder().createArrayType(m_Length, 0, m_Base->GenDI(builder), {});
}

QLang::TypePtr QLang::ArrayType::GetBase() const { return m_Base; }

uint64_t QLang::ArrayType::GetLength() const { return m_Length; }
