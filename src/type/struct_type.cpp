#include <memory>
#include <utility>
#include <llvm/IR/DerivedTypes.h>
#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Type.hpp>

QLang::StructTypePtr QLang::StructType::From(const TypePtr& type)
{
    return std::dynamic_pointer_cast<StructType>(type);
}

QLang::StructTypePtr QLang::StructType::Get(Context& ctx, const std::string& name)
{
    auto& ref = ctx.GetType("struct " + name);
    if (!ref)
    {
        std::vector<StructElement> elements;
        ref = std::make_shared<StructType>(ctx, "struct " + name, name, 0, elements);
    }
    return From(ref);
}

QLang::StructTypePtr QLang::StructType::Get(const std::string& name, std::vector<StructElement>& elements)
{
    size_t size = 0;

    auto full_name = "struct " + name;
    if (!name.empty())
    {
        for (auto& [Type, Name, Init] : elements) size += Type->GetSize();
    }
    else
    {
        full_name += "{ ";
        for (size_t i = 0; i < elements.size(); ++i)
        {
            size += elements[i].Type->GetSize();
            if (i > 0) full_name += ", ";
            full_name += elements[i].Type->GetName();
        }
        full_name += " }";
    }

    auto& ctx = elements[0].Type->GetCtx();
    auto& ref = ctx.GetType(full_name);
    if (!ref)
        ref = std::make_shared<StructType>(ctx, full_name, name, size, elements);
    return From(ref);
}

QLang::StructType::StructType(Context& ctx,
                              const std::string& name,
                              std::string struct_name,
                              const size_t size,
                              std::vector<StructElement>& elements)
    : Type(ctx, name, TypeId_Struct, size),
      m_StructName(std::move(struct_name)),
      m_Elements(std::move(elements))
{
}

llvm::StructType* QLang::StructType::GenIR(Builder& builder)
{
    if (m_IR)
        return m_IR;

    auto type = llvm::StructType::getTypeByName(builder.IRContext(), m_StructName);
    if (!type)
        type = llvm::StructType::create(builder.IRContext(), m_StructName);

    if (m_Elements.empty()) return m_IR = type;

    if (type->isEmptyTy())
    {
        std::vector<llvm::Type*> elements;
        elements.reserve(m_Elements.size());
        for (const auto& [Type, Name, Init] : m_Elements)
            elements.push_back(Type->GenIR(builder));
        type->setBody(elements);
    }

    return m_IR = type;
}

llvm::DIType* QLang::StructType::GenDI(Builder& builder)
{
    if (m_DI)
        return m_DI;

    const auto file = builder.Scope()->getFile();

    std::vector<llvm::Metadata*> elements;
    size_t max_size = 0;
    size_t offset = 0;
    for (size_t i = 0; i < m_Elements.size(); ++i)
    {
        auto& [type_, name_, init_] = m_Elements[i];
        if (type_->GetSize() > max_size) max_size = type_->GetSize();
        const auto element = builder.DIBuilder().createMemberType(
            {},
            name_,
            file,
            i + 1,
            type_->GetSize(),
            0,
            offset,
            llvm::DINode::FlagZero,
            type_->GenDI(builder));
        offset += type_->GetSize();
        elements.push_back(element);
    }

    return m_DI = builder.DIBuilder().createStructType(
        {},
        m_StructName,
        file,
        1,
        GetSize(),
        max_size,
        llvm::DINode::FlagZero,
        {},
        builder.DIBuilder().getOrCreateArray(elements));
}

size_t QLang::StructType::GetElementCount() const { return m_Elements.size(); }

const QLang::StructElement& QLang::StructType::GetElement(const size_t index) const
{
    return m_Elements[index];
}
