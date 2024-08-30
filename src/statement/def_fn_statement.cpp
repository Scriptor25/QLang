#include <QLang/Builder.hpp>
#include <QLang/Function.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

QLang::DefFnStatement::DefFnStatement(
	const SourceLocation &where, FnMode mode, const TypePtr &result,
	const TypePtr &self, const std::string &name,
	const std::vector<Param> &params, bool vararg, StatementPtr body)
	: Statement(where), Mode(mode), Result(result), Self(self), Name(name),
	  Params(params), VarArg(vararg), Body(std::move(body))
{
}

std::ostream &QLang::DefFnStatement::Print(std::ostream &stream) const
{
	stream << "def ";
	if (Mode == FnMode_Func)
	{
		stream << Result << ' ';
		if (Self) stream << Self << ':';
	}
	else if (Mode == FnMode_Ctor) { stream << '+'; }
	else if (Mode == FnMode_Dtor) { stream << '-'; }
	stream << Name << '(';
	for (size_t i = 0; i < Params.size(); ++i)
	{
		if (i > 0) stream << ", ";
		stream << Params[i].Type;
		if (!Params[i].Name.empty()) stream << ' ' << Params[i].Name;
	}
	if (VarArg)
	{
		if (!Params.empty()) stream << ", ";
		stream << '?';
	}
	stream << ')';
	if (!Body) return stream;
	return stream << ' ' << Body;
}

void QLang::DefFnStatement::GenIRVoid(Builder &builder) const
{
	std::vector<TypePtr> param_types;
	for (const auto &param : Params) param_types.push_back(param.Type);
	auto type = FunctionType::Get(Mode, Result, Self, param_types, VarArg);

	auto &ref = builder.GetFunction(Name, type);
	if (!ref.IR)
	{
		ref.Name = Name;
		ref.Type = type;
		ref.IRType = type->GenIR(builder);
		ref.IR = llvm::Function::Create(
			ref.IRType, llvm::GlobalValue::ExternalLinkage, Name,
			builder.IRModule());
	}

	if (!Body) return;
	if (!ref.IR->empty())
	{
		std::cerr << "at " << Where << ": cannot redefine function"
				  << std::endl;
		return;
	}

	auto bb = llvm::BasicBlock::Create(builder.IRContext(), "entry", ref.IR);
	builder.IRBuilder().SetInsertPoint(bb);

	builder.StackPush();
	builder.GetResult() = type->GetResult();
	builder.DestroyAtEnd().clear();

	unsigned off = Self ? 1 : 0;
	if (off)
	{
		auto arg = ref.IR->getArg(0);
		arg->setName("self");
		builder["self"] = LValue::Create(builder, Self, arg);
	}

	for (size_t i = 0; i < Params.size(); ++i)
	{
		auto &[arg_type, arg_name] = Params[i];

		auto arg = ref.IR->getArg(i + off);
		arg->setName(arg_name);

		if (auto aty = ReferenceType::From(arg_type))
			builder[arg_name] = LValue::Create(builder, aty->GetBase(), arg);
		else
		{
			builder[arg_name]
				= LValue::Alloca(builder, arg_type, arg, arg_name);
		}
	}

	Body->GenIRVoid(builder);
	builder.StackPop();

	if (type->GetResult()->IsVoid())
		for (auto &block : *ref.IR)
		{
			if (block.getTerminator()) continue;
			builder.IRBuilder().SetInsertPoint(&block);
			builder.IRBuilder().CreateRetVoid();
		}

	for (auto &block : *ref.IR)
	{
		auto terminator = block.getTerminator();
		if (!terminator || !terminator->willReturn()) continue;

		builder.IRBuilder().SetInsertPoint(terminator);
		for (auto &value : builder.DestroyAtEnd())
			if (auto func = builder.FindDestructor(value->GetType()))
				GenCall(
					builder, func->AsValue(builder), LValue::From(value), {});
	}

	builder.IRBuilder().ClearInsertionPoint();

	if (llvm::verifyFunction(*ref.IR, &llvm::errs()))
	{
		ref.IR->print(llvm::errs());
		ref.IR->erase(ref.IR->begin(), ref.IR->end());
		return;
	}

	builder.Optimize(ref.IR);
	ref.IR->print(llvm::errs());
}
