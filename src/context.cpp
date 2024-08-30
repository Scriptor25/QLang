#include <QLang/Context.hpp>
#include <QLang/Type.hpp>
#include <memory>

QLang::Context::Context()
{
	m_Types["void"] = std::make_shared<Type>(*this, "void", TypeId_Void, 0);
	m_Types["i1"] = std::make_shared<Type>(*this, "i1", TypeId_Int, 1);
	m_Types["i8"] = std::make_shared<Type>(*this, "i8", TypeId_Int, 8);
	m_Types["i16"] = std::make_shared<Type>(*this, "i16", TypeId_Int, 16);
	m_Types["i32"] = std::make_shared<Type>(*this, "i32", TypeId_Int, 32);
	m_Types["i64"] = std::make_shared<Type>(*this, "i64", TypeId_Int, 64);
	m_Types["f16"] = std::make_shared<Type>(*this, "f16", TypeId_Float, 16);
	m_Types["f32"] = std::make_shared<Type>(*this, "f32", TypeId_Float, 32);
	m_Types["f64"] = std::make_shared<Type>(*this, "f64", TypeId_Float, 64);
}

QLang::TypePtr &QLang::Context::GetType(const std::string &name)
{
	return m_Types[name];
}
