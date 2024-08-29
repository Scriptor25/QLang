#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Type.hpp>
#include <llvm/IR/DerivedTypes.h>
#include <memory>

QLang::FunctionTypePtr QLang::FunctionType::From(const TypePtr &type)
{
	return std::dynamic_pointer_cast<FunctionType>(type);
}

QLang::FunctionTypePtr QLang::FunctionType::FromPtr(const TypePtr &type)
{
	return From(PointerType::From(type)->GetBase());
}

QLang::FunctionTypePtr QLang::FunctionType::Get(
	FnMode mode, const TypePtr &result, const TypePtr &self,
	const std::vector<TypePtr> &params, bool vararg)
{
	std::string name;
	name += result->GetName();
	if (self)
	{
		name += '(';
		if (mode == FnMode_Func) name += ':';
		else if (mode == FnMode_Ctor)
			name += '+';
		else if (mode == FnMode_Dtor)
			name += '-';
		name += self->GetName();
		name += ')';
	}
	name += '(';
	for (size_t i = 0; i < params.size(); ++i)
	{
		if (i > 0) name += ',';
		name += params[i]->GetName();
	}
	if (vararg)
	{
		if (!params.empty()) name += ',';
		name += '?';
	}
	name += ')';

	auto &ctx = result->GetCtx();
	auto &ref = ctx.GetType(name);
	if (!ref)
		ref = std::make_shared<FunctionType>(
			ctx, name, mode, result, self, params, vararg);
	return FunctionType::From(ref);
}

QLang::FunctionType::FunctionType(
	Context &ctx, const std::string &name, FnMode mode, const TypePtr &result,
	const TypePtr &self, const std::vector<TypePtr> &params, bool vararg)
	: Type(ctx, name, TypeId_Function, 0), m_Mode(mode), m_Result(result),
	  m_Self(self), m_Params(params), m_VarArg(vararg)
{
}

llvm::FunctionType *QLang::FunctionType::GenIR(Builder &builder) const
{
	std::vector<llvm::Type *> params;
	if (m_Self) params.push_back(builder.IRBuilder().getPtrTy());
	for (const auto &param : m_Params) params.push_back(param->GenIR(builder));
	return llvm::FunctionType::get(m_Result->GenIR(builder), params, m_VarArg);
}

QLang::FnMode QLang::FunctionType::GetMode() const { return m_Mode; }

QLang::TypePtr QLang::FunctionType::GetResult() const { return m_Result; }

QLang::TypePtr QLang::FunctionType::GetSelf() const { return m_Self; }

size_t QLang::FunctionType::GetParamCount() const { return m_Params.size(); }

QLang::TypePtr QLang::FunctionType::GetParam(size_t i) const
{
	return m_Params[i];
}

bool QLang::FunctionType::IsVarArg() const { return m_VarArg; }
