#include <iostream>
#include <utility>
#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Type.hpp>

std::ostream& QLang::operator<<(std::ostream& stream, const TypePtr& ptr)
{
    return stream << ptr->GetName();
}

QLang::TypePtr QLang::Type::Get(Context& ctx, const std::string& name)
{
    if (auto& ref = ctx.GetType(name)) return ref;
    std::cerr << "no such type '" << name << "'" << std::endl;
    return {};
}

size_t QLang::Type::TypeDiff(Builder& builder, TypePtr p, TypePtr a)
{
    if (p->IsVoid() || a->IsVoid()) return 0;

    if (p->IsReference()) p = ReferenceType::From(p)->GetBase();
    if (a->IsReference()) a = ReferenceType::From(a)->GetBase();

    if (p == a) return 0;

    if (builder.FindFunction("operator$" + p->GetName(), a, {})) return 16;

    if (p->IsPointer() && a->IsPointer())
    {
        p = PointerType::From(p)->GetBase();
        a = PointerType::From(a)->GetBase();
        return TypeDiff(builder, p, a);
    }

    if (p->IsPointer() && a->IsArray())
    {
        p = PointerType::From(p)->GetBase();
        a = ArrayType::From(a)->GetBase();
        return TypeDiff(builder, p, a);
    }

    if (p->IsArray() ||
        a->IsArray() ||
        p->IsReference() ||
        a->IsReference() ||
        p->IsStruct() ||
        a->IsStruct() ||
        p->IsFunction() ||
        a->IsFunction())
        return -1;

    if (p->m_Id == a->m_Id)
    {
        const auto as = p->m_Size;
        const auto bs = a->m_Size;
        return as > bs ? as - bs : bs - as;
    }

    if (p->IsFloat() && a->IsInt()) return 16;
    if (p->IsInt() && a->IsFloat()) return 32;

    return -1;
}

QLang::TypePtr QLang::Type::HigherOrder(const TypePtr& a, const TypePtr& b)
{
    if (a == b) return a;

    switch (a->m_Id)
    {
    case TypeId_Int:
        switch (b->m_Id)
        {
        case TypeId_Int:
            if (a->m_Size > b->m_Size) return a;
            return b;
        case TypeId_Float: return b;
        case TypeId_Pointer: return a;
        default: break;
        }
        break;
    case TypeId_Float:
        switch (b->m_Id)
        {
        case TypeId_Float:
            if (a->m_Size > b->m_Size) return a;
            return b;
        case TypeId_Int:
        case TypeId_Pointer: return a;
        default: break;
        }
        break;
    case TypeId_Pointer:
        switch (b->m_Id)
        {
        case TypeId_Int:
        case TypeId_Float: return a;
        case TypeId_Pointer: return Get(a->m_Ctx, "i64");
        default: break;
        }
        break;
    default: break;
    }

    std::cerr << "cannot determine higher order type of " << a << " and " << b
        << std::endl;
    return {};
}

QLang::Type::~Type() = default;

llvm::Type* QLang::Type::GenIR(Builder& builder)
{
    if (m_IR)
        return m_IR;

    switch (m_Id)
    {
    case TypeId_Void:
        m_IR = builder.IRBuilder().getVoidTy();
        break;
    case TypeId_Int:
        m_IR = builder.IRBuilder().getIntNTy(m_Size);
        break;
    case TypeId_Float:
        switch (m_Size)
        {
        case 16:
            m_IR = builder.IRBuilder().getHalfTy();
            break;
        case 32:
            m_IR = builder.IRBuilder().getFloatTy();
            break;
        case 64:
            m_IR = builder.IRBuilder().getDoubleTy();
            break;
        default: break;
        }
        break;
    default: break;
    }

    return m_IR;
}

llvm::DIType* QLang::Type::GenDI(Builder& builder)
{
    if (m_DI)
        return m_DI;

    switch (m_Id)
    {
    case TypeId_Void:
        m_DI = builder.DIBuilder().createBasicType(m_Name, m_Size, 0);
        break;
    case TypeId_Int:
        m_DI = builder.DIBuilder().createBasicType(m_Name, m_Size, llvm::dwarf::DW_ATE_signed);
        break;
    case TypeId_Float:
        m_DI = builder.DIBuilder().createBasicType(m_Name, m_Size, llvm::dwarf::DW_ATE_float);
        break;
    default: break;
    }

    return m_DI;
}

QLang::Type::Type(Context& ctx, std::string name, const TypeId id, const size_t size)
    : m_Ctx(ctx), m_Name(std::move(name)), m_Id(id), m_Size(size)
{
}

QLang::Context& QLang::Type::GetCtx() const { return m_Ctx; }

std::string QLang::Type::GetName() const { return m_Name; }

QLang::TypeId QLang::Type::GetId() const { return m_Id; }

size_t QLang::Type::GetSize() const { return m_Size; }

bool QLang::Type::IsVoid() const { return m_Id == TypeId_Void; }

bool QLang::Type::IsInt() const { return m_Id == TypeId_Int; }

bool QLang::Type::IsFloat() const { return m_Id == TypeId_Float; }

bool QLang::Type::IsPointer() const { return m_Id == TypeId_Pointer; }

bool QLang::Type::IsReference() const { return m_Id == TypeId_Reference; }

bool QLang::Type::IsArray() const { return m_Id == TypeId_Array; }

bool QLang::Type::IsStruct() const { return m_Id == TypeId_Struct; }

bool QLang::Type::IsFunction() const { return m_Id == TypeId_Function; }

bool QLang::Type::IsFunctionPointer() const
{
    return IsPointer() && dynamic_cast<const PointerType*>(this)->GetBase()->IsFunction();
}
