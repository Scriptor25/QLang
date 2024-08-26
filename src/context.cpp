#include <QLang/Context.hpp>
#include <QLang/Type.hpp>
#include <memory>

QLang::Context::Context()
{
	m_Types["void"] = std::make_shared<Type>(*this, "void", 0);
	m_Types["i1"] = std::make_shared<Type>(*this, "i1", 1);
	m_Types["i8"] = std::make_shared<Type>(*this, "i8", 8);
	m_Types["i16"] = std::make_shared<Type>(*this, "i16", 16);
	m_Types["i32"] = std::make_shared<Type>(*this, "i32", 32);
	m_Types["i64"] = std::make_shared<Type>(*this, "i64", 64);
	m_Types["f16"] = std::make_shared<Type>(*this, "f16", 16);
	m_Types["f32"] = std::make_shared<Type>(*this, "f32", 32);
	m_Types["f64"] = std::make_shared<Type>(*this, "f64", 64);
}

QLang::TypePtr &QLang::Context::GetType(const std::string &name)
{
	return m_Types[name];
}
