#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <vector>

QLang::ValuePtr QLang::GenCall(
	Builder &builder, const ValuePtr &callee, const LValuePtr &self,
	const std::vector<ValuePtr> &args)
{
	auto type = FunctionType::FromPtr(callee->GetType());
	auto ir_type = type->GenIR(builder);

	std::vector<llvm::Value *> ir_args;
	for (size_t i = 0; i < args.size(); ++i)
	{
		auto arg = args[i];
		if (i < type->GetParamCount())
		{

			if (type->GetParam(i)->IsReference())
			{
				ir_args.push_back(LValue::From(arg)->GetPtr());
				continue;
			}

			arg = GenCast(builder, arg, type->GetParam(i));
			if (!arg) return {};

			ir_args.push_back(arg->Get());
			continue;
		}

		ir_args.push_back(arg->Get());
	}

	if (type->GetMode() == FnMode_Ctor)
	{
		auto inst = LValue::Alloca(builder, type->GetSelf());
		ir_args.insert(ir_args.begin(), inst->GetPtr());
		builder.IRBuilder().CreateCall(ir_type, callee->Get(), ir_args);
		return inst;
	}

	if (type->GetSelf()) ir_args.insert(ir_args.begin(), self->GetPtr());

	auto result
		= builder.IRBuilder().CreateCall(ir_type, callee->Get(), ir_args);

	if (auto ref_type = ReferenceType::From(type->GetResult()))
		return LValue::Create(builder, ref_type->GetBase(), result);

	return RValue::Create(builder, type->GetResult(), result);
}
