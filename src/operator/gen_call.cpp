#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <vector>

QLang::ValuePtr QLang::GenCall(
	Builder &builder, const ValuePtr &callee, const LValuePtr &self,
	const std::vector<ValuePtr> &args)
{
	auto func_type = FunctionType::FromPtr(callee->GetType());
	auto fn_ty = func_type->GenIR(builder);

	std::vector<llvm::Value *> vargs;
	for (size_t i = 0; i < args.size(); ++i)
	{
		auto arg = args[i];
		if (i < func_type->GetParamCount())
		{
			if (func_type->GetParam(i)->IsReference())
			{
				vargs.push_back(LValue::From(arg)->GetPtr());
				continue;
			}

			arg = GenCast(builder, arg, func_type->GetParam(i));
			if (!arg) return {};

			vargs.push_back(arg->Get());
			continue;
		}

		vargs.push_back(arg->Get());
	}

	if (func_type->GetMode() == FnMode_Ctor)
	{
		auto inst = self ? self : builder.CreateInstance(func_type->GetSelf());
		vargs.insert(vargs.begin(), inst->GetPtr());
		builder.IRBuilder().CreateCall(fn_ty, callee->Get(), vargs);
		return inst;
	}

	if (func_type->GetSelf()) vargs.insert(vargs.begin(), self->GetPtr());

	auto result = builder.IRBuilder().CreateCall(fn_ty, callee->Get(), vargs);

	if (auto ref_type = ReferenceType::From(func_type->GetResult()))
		return LValue::Create(builder, ref_type->GetBase(), result);

	return RValue::Create(builder, func_type->GetResult(), result);
}
