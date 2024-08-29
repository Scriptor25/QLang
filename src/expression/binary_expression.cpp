#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Value.hpp>
#include <iostream>
#include <memory>

QLang::BinaryExpression::BinaryExpression(
	const SourceLocation &where, const std::string &operator_,
	ExpressionPtr lhs, ExpressionPtr rhs)
	: Expression(where), Operator(operator_), LHS(std::move(lhs)),
	  RHS(std::move(rhs))
{
}

std::ostream &QLang::BinaryExpression::Print(std::ostream &stream) const
{
	if (Operator == "[]") return stream << LHS << '[' << RHS << ']';
	return stream << LHS << ' ' << Operator << ' ' << RHS;
}

QLang::ValuePtr QLang::BinaryExpression::GenIR(Builder &builder) const
{
	auto lhs = LHS->GenIR(builder);
	if (!lhs) return {};
	auto rhs = RHS->GenIR(builder);
	if (!rhs) return {};

	auto self = LValue::From(lhs);

	if (self)
		if (auto func = builder.FindFunction(
				"operator" + Operator, lhs->GetType(), { rhs->GetType() }))
			return GenCall(builder, func->AsValue(builder), self, { rhs });

	if (auto func = builder.FindFunction(
			"operator" + Operator, {}, { lhs->GetType(), rhs->GetType() }))
		return GenCall(builder, func->AsValue(builder), {}, { lhs, rhs });

	if (Operator == "=")
	{
		rhs = GenCast(builder, rhs, self->GetType());
		if (!rhs) return {};
		self->Set(rhs->Get());
		return self;
	}

	if (Operator == "[]") return GenSubscript(builder, lhs, rhs);

	std::string op = Operator;
	bool assign = false;
	ValuePtr result;

	if (!(op == "==" || op == "!=" || op == "<=" || op == ">=")
		&& op.back() == '=')
	{
		op.pop_back();
		assign = true;
	}

	auto higher = Type::HigherOrder(lhs->GetType(), rhs->GetType());
	lhs = GenCast(builder, lhs, higher);
	if (!lhs) return {};
	rhs = GenCast(builder, rhs, higher);
	if (!rhs) return {};

	if (op == "+") { result = GenAdd(builder, lhs, rhs); }
	else if (op == "-") { result = GenSub(builder, lhs, rhs); }
	else if (op == "*") { result = GenMul(builder, lhs, rhs); }
	else if (op == "/") { result = GenDiv(builder, lhs, rhs); }
	else if (op == "%") { result = GenRem(builder, lhs, rhs); }
	else if (op == "&") { result = GenAnd(builder, lhs, rhs); }
	else if (op == "|") { result = GenOr(builder, lhs, rhs); }
	else if (op == "^") { result = GenXor(builder, lhs, rhs); }
	else if (op == "==") { result = GenEQ(builder, lhs, rhs); }
	else if (op == "!=") { result = GenNE(builder, lhs, rhs); }
	else if (op == "<=") { result = GenLE(builder, lhs, rhs); }
	else if (op == ">=") { result = GenGE(builder, lhs, rhs); }
	else if (op == "<") { result = GenLT(builder, lhs, rhs); }
	else if (op == ">") { result = GenGT(builder, lhs, rhs); }
	else if (op == "&&") { result = GenLAnd(builder, lhs, rhs); }
	else if (op == "||") { result = GenLOr(builder, lhs, rhs); }
	else if (op == "^^") { result = GenLXor(builder, lhs, rhs); }
	else if (op == "<<") { result = GenShl(builder, lhs, rhs); }
	else if (op == ">>") { result = GenLShr(builder, lhs, rhs); }
	else if (op == ">>>") { result = GenAShr(builder, lhs, rhs); }

	if (!result)
	{
		std::cerr << "QLang::BinaryExpression::GenIR" << std::endl;
		return {};
	}

	if (assign)
	{
		result = GenCast(builder, result, self->GetType());
		if (!result) return {};
		self->Set(result->Get());
		return self;
	}

	return result;
}
