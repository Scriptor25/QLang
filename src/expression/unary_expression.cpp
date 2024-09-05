#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Value.hpp>
#include <iostream>
#include <utility>

QLang::UnaryExpression::UnaryExpression(
	const SourceLocation &where, const std::string &operator_,
	StatementPtr operand, const bool post)
	: UnaryExpression(where, operator_, dyn_cast<Expression>(operand), post)
{
}

QLang::UnaryExpression::UnaryExpression(
	const SourceLocation &where, std::string operator_, ExpressionPtr operand,
	const bool post)
	: Expression(where), Operator(std::move(operator_)),
	  Operand(std::move(operand)), Post(post)
{
}

std::ostream &QLang::UnaryExpression::Print(std::ostream &stream) const
{
	return stream << '(' << (Post ? "" : Operator) << Operand
				  << (Post ? Operator : "") << ')';
}

QLang::ValuePtr QLang::UnaryExpression::GenIR(Builder &builder) const
{
	auto operand = Operand->GenIR(builder);
	if (!operand)
	{
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

	auto self = LValue::From(operand);
	const auto func_name = "operator" + Operator + (Post ? "^" : "");

	if (self)
		if (const auto func
			= builder.FindFunction(func_name, self->GetType(), {}))
		{
			if (auto result
				= GenCall(builder, func->AsValue(builder), self, {}))
				return result;
			std::cerr << "    at " << Where << std::endl;
			return {};
		}

	if (const auto func
		= builder.FindFunction(func_name, {}, { operand->GetType() }))
	{
		if (auto result
			= GenCall(builder, func->AsValue(builder), {}, { operand }))
			return result;
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

	ValuePtr result;
	bool assign = false;

	if (Operator == "++")
	{
		result = GenInc(builder, operand);
		assign = true;
	}
	else if (Operator == "--")
	{
		result = GenDec(builder, operand);
		assign = true;
	}
	else if (Operator == "!") { result = GenLNot(builder, operand); }
	else if (Operator == "~") { result = GenNot(builder, operand); }
	else if (Operator == "-") { result = GenNeg(builder, operand); }
	else if (Operator == "&") { result = GenRef(builder, operand); }
	else if (Operator == "*") { result = GenDeref(builder, operand); }

	if (!result)
	{
		std::cerr << "at " << Where << ": TODO" << std::endl;
		return {};
	}

	if (assign)
	{
		if (!self)
		{
			std::cerr << "at " << Where << ": operand must be a lvalue here"
					  << std::endl;
			return {};
		}

		if (Post)
		{
			const auto pre = self->Get();
			self->Set(result->Get());
			return RValue::Create(builder, self->GetType(), pre);
		}

		self->Set(result->Get());
		return self;
	}

	return result;
}
