#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Type.hpp>
#include <iostream>

std::ostream &QLang::operator<<(std::ostream &stream, const TypePtr &ptr)
{
	return stream << ptr->GetName();
}

QLang::TypePtr QLang::Type::Get(Context &ctx, const std::string &name)
{
	return ctx.GetType(name);
}

size_t QLang::Type::TypeDiff(Builder &builder, TypePtr a, TypePtr b)
{
	if (a->IsVoid() || b->IsVoid()) return 0;

	if (a->IsReference()) a = ReferenceType::From(a)->GetBase();
	if (b->IsReference()) b = ReferenceType::From(b)->GetBase();

	if (a == b) return 0;

	if (builder.FindFunction("operator$" + a->GetName(), b, {})) return 1;
	if (builder.FindConstructor(b, { a })) return 2;

	if (a->IsPointer() && b->IsPointer())
	{
		a = PointerType::From(a)->GetBase();
		b = PointerType::From(b)->GetBase();
		return TypeDiff(builder, a, b);
	}

	if (a->IsArray() || b->IsArray() || a->IsReference() || b->IsReference()
		|| a->IsStruct() || b->IsStruct() || a->IsFunction() || b->IsFunction())
		return 1000;

	if (a->m_Id == b->m_Id)
	{
		auto as = a->m_Size;
		auto bs = b->m_Size;
		return as > bs ? as - bs : bs - as;
	}

	return 1000;
}

QLang::TypePtr QLang::Type::HigherOrder(const TypePtr &a, const TypePtr &b)
{
	if (a == b) return a;

	std::cerr << "QLang::Type::HigherOrder" << std::endl;
	return {};
}

QLang::Type::~Type() = default;

llvm::Type *QLang::Type::GenIR(Builder &builder) const
{
	switch (m_Id)
	{
	case TypeId_Void: return builder.IRBuilder().getVoidTy();
	case TypeId_Int: return builder.IRBuilder().getIntNTy(m_Size);
	case TypeId_Float:
		switch (m_Size)
		{
		case 16: return builder.IRBuilder().getHalfTy();
		case 32: return builder.IRBuilder().getFloatTy();
		case 64: return builder.IRBuilder().getDoubleTy();
		default: break;
		}
		break;
	default: break;
	}
	return nullptr;
}

QLang::Type::Type(Context &ctx, const std::string &name, TypeId id, size_t size)
	: m_Ctx(ctx), m_Name(name), m_Id(id), m_Size(size)
{
}

QLang::Context &QLang::Type::GetCtx() const { return m_Ctx; }

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
	return IsPointer()
		   && dynamic_cast<const PointerType *>(this)->GetBase()->IsFunction();
}
