#include <QLang/Builder.hpp>
#include <QLang/Type.hpp>
#include <memory>

QLang::Builder::Builder(Context &context) : m_Context(context)
{
	m_IRContext = std::make_unique<llvm::LLVMContext>();
	m_IRBuilder = std::make_unique<llvm::IRBuilder<>>(*m_IRContext);
	m_IRModule = std::make_unique<llvm::Module>("module", *m_IRContext);
}

llvm::LLVMContext &QLang::Builder::IRContext() const { return *m_IRContext; }

llvm::IRBuilder<> &QLang::Builder::IRBuilder() const { return *m_IRBuilder; }

llvm::Module &QLang::Builder::IRModule() const { return *m_IRModule; }

void QLang::Builder::Push() {}

void QLang::Builder::Pop() {}

QLang::TypePtr QLang::Builder::GetVoidTy() const
{
	return Type::Get(m_Context, "void");
}

QLang::TypePtr QLang::Builder::GetInt1Ty() const
{
	return Type::Get(m_Context, "i1");
}

QLang::TypePtr QLang::Builder::GetInt8Ty() const
{
	return Type::Get(m_Context, "i8");
}

QLang::TypePtr QLang::Builder::GetInt16Ty() const
{
	return Type::Get(m_Context, "i16");
}

QLang::TypePtr QLang::Builder::GetInt32Ty() const
{
	return Type::Get(m_Context, "i32");
}

QLang::TypePtr QLang::Builder::GetInt64Ty() const
{
	return Type::Get(m_Context, "i64");
}

QLang::TypePtr QLang::Builder::GetFloat16Ty() const
{
	return Type::Get(m_Context, "f16");
}

QLang::TypePtr QLang::Builder::GetFloat32Ty() const
{
	return Type::Get(m_Context, "f32");
}

QLang::TypePtr QLang::Builder::GetFloat64Ty() const
{
	return Type::Get(m_Context, "f64");
}

QLang::PointerTypePtr QLang::Builder::GetVoidPtrTy() const
{
	return PointerType::Get(GetVoidTy());
}

QLang::PointerTypePtr QLang::Builder::GetInt8PtrTy() const
{
	return PointerType::Get(GetInt8Ty());
}
