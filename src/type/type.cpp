#include <QLang/Context.hpp>
#include <QLang/Type.hpp>

std::ostream &QLang::operator<<(std::ostream &stream, const TypePtr &ptr)
{
	return stream << ptr->GetName();
}

QLang::TypePtr QLang::Type::Get(Context &ctx, const std::string &name)
{
	return ctx.GetType(name);
}

QLang::Type::~Type() = default;

llvm::Type *QLang::Type::GenIR(Builder &builder) const {}

QLang::Type::Type(Context &ctx, const std::string &name, size_t size)
	: m_Ctx(ctx), m_Name(name), m_Size(size)
{
}

QLang::Context &QLang::Type::GetCtx() const { return m_Ctx; }

const std::string &QLang::Type::GetName() const { return m_Name; }

size_t QLang::Type::GetSize() const { return m_Size; }
