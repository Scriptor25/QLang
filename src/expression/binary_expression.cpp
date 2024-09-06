#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Value.hpp>
#include <iostream>
#include <memory>
#include <utility>

QLang::BinaryExpression::BinaryExpression(
	const SourceLocation &where, const std::string &operator_, StatementPtr lhs,
	StatementPtr rhs)
	: BinaryExpression(where, operator_, dyn_cast<Expression>(lhs),
					   dyn_cast<Expression>(rhs))
{
}

QLang::BinaryExpression::BinaryExpression(
	const SourceLocation &where, std::string operator_, ExpressionPtr lhs,
	ExpressionPtr rhs)
	: Expression(where), Operator(std::move(operator_)), LHS(std::move(lhs)),
	  RHS(std::move(rhs))
{
}

bool QLang::BinaryExpression::IsConstant() const
{
	return LHS->IsConstant() && RHS->IsConstant();
}

std::ostream &QLang::BinaryExpression::Print(std::ostream &stream) const
{
	if (Operator == "[]") return stream << LHS << '[' << RHS << ']';
	return stream << '(' << LHS << ' ' << Operator << ' ' << RHS << ')';
}

QLang::ValuePtr QLang::BinaryExpression::GenIR(Builder &builder) const
{
	const auto bkp = builder.IsCallee();
	builder.ClearCallee();

	auto lhs = LHS->GenIR(builder);
	if (!lhs)
	{
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

	if (bkp) builder.SetCallee();

	if (Operator == ".")
	{
		if (auto result = GenMember(
				builder, lhs, false,
				dynamic_cast<const NameExpression *>(RHS.get())->Name))
			return result;
		std::cerr << "    at " << Where << std::endl;
		return {};
	}
	if (Operator == "!")
	{
		if (auto result = GenMember(
				builder, lhs, false,
				dynamic_cast<const NameExpression *>(RHS.get())->Name))
			return result;
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

	auto rhs = RHS->GenIR(builder);
	if (!rhs)
	{
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

	auto self = LValue::From(lhs);

	if (self)
		if (const auto func = builder.FindFunction(
				"operator" + Operator, lhs->GetType(), { rhs->GetType() }))
		{
			if (auto result
				= GenCall(builder, func->AsValue(builder), self, { rhs }))
				return result;
			std::cerr << "    at " << Where << std::endl;
			return {};
		}

	if (const auto func = builder.FindFunction(
			"operator" + Operator, {}, { lhs->GetType(), rhs->GetType() }))
	{
		if (auto result
			= GenCall(builder, func->AsValue(builder), {}, { lhs, rhs }))
			return result;
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

	if (Operator == "=")
	{
		if (!self)
		{
			std::cerr << "at " << Where << ": lhs must be a lvalue here"
					  << std::endl;
			return {};
		}

		rhs = GenCast(builder, rhs, self->GetType());
		if (!rhs)
		{
			std::cerr << "    at " << Where << std::endl;
			return {};
		}

		self->Set(rhs->Get());
		return self;
	}

	if (Operator == "[]")
	{
		if (auto result = GenSubscript(builder, lhs, rhs)) return result;
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

	std::string op = Operator;
	bool assign = false;
	ValuePtr result;

	if (!(op == "==" || op == "!=" || op == "<=" || op == ">=")
		&& op.back() == '=')
	{
		op.pop_back();
		assign = true;
	}

	const auto higher = Type::HigherOrder(lhs->GetType(), rhs->GetType());
	if (!higher)
	{
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

	lhs = GenCast(builder, lhs, higher);
	if (!lhs)
	{
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

	rhs = GenCast(builder, rhs, higher);
	if (!rhs)
	{
		std::cerr << "    at " << Where << std::endl;
		return {};
	}

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
		std::cerr << "at " << Where << ": TODO" << std::endl;
		return {};
	}

	if (assign)
	{
		if (!self)
		{
			std::cerr << "at " << Where << ": lhs must be a lvalue here"
					  << std::endl;
			return {};
		}

		result = GenCast(builder, result, self->GetType());
		if (!result)
		{
			std::cerr << "    at " << Where << std::endl;
			return {};
		}

		self->Set(result->Get());
		return self;
	}

	return result;
}

#define COMPUTE_IFC(WHERE, LHS, O, RHS)                                        \
	do {                                                                       \
		const auto lhs_int = dynamic_cast<ConstIntExpression *>((LHS).get());  \
		const auto rhs_int = dynamic_cast<ConstIntExpression *>((RHS).get());  \
		const auto lhs_float                                                   \
			= dynamic_cast<ConstFloatExpression *>((LHS).get());               \
		const auto rhs_float                                                   \
			= dynamic_cast<ConstFloatExpression *>((RHS).get());               \
		const auto lhs_char                                                    \
			= dynamic_cast<ConstCharExpression *>((LHS).get());                \
		const auto rhs_char                                                    \
			= dynamic_cast<ConstCharExpression *>((RHS).get());                \
                                                                               \
		if (lhs_int)                                                           \
		{                                                                      \
			if (rhs_int)                                                       \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_int->Value O rhs_int->Value);                 \
			if (rhs_float)                                                     \
				return std::make_unique<ConstFloatExpression>(                 \
					(WHERE),                                                   \
					static_cast<double>(lhs_int->Value) O rhs_float->Value);   \
			if (rhs_char)                                                      \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_int->Value O rhs_char->Value);                \
			return {};                                                         \
		}                                                                      \
		if (lhs_float)                                                         \
		{                                                                      \
			if (rhs_int)                                                       \
				return std::make_unique<ConstFloatExpression>(                 \
					(WHERE),                                                   \
					lhs_float->Value O static_cast<double>(rhs_int->Value));   \
			if (rhs_float)                                                     \
				return std::make_unique<ConstFloatExpression>(                 \
					(WHERE), lhs_float->Value O rhs_float->Value);             \
			if (rhs_char)                                                      \
				return std::make_unique<ConstFloatExpression>(                 \
					(WHERE), lhs_float->Value O rhs_char->Value);              \
			return {};                                                         \
		}                                                                      \
		if (lhs_char)                                                          \
		{                                                                      \
			if (rhs_int)                                                       \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_char->Value O rhs_int->Value);                \
			if (rhs_float)                                                     \
				return std::make_unique<ConstFloatExpression>(                 \
					(WHERE), lhs_char->Value O rhs_float->Value);              \
			if (rhs_char)                                                      \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_char->Value O rhs_char->Value);               \
			return {};                                                         \
		}                                                                      \
		return {};                                                             \
	} while (0)

#define COMPUTE_IC(WHERE, LHS, O, RHS)                                         \
	do {                                                                       \
		const auto lhs_int = dynamic_cast<ConstIntExpression *>((LHS).get());  \
		const auto rhs_int = dynamic_cast<ConstIntExpression *>((RHS).get());  \
		const auto lhs_char                                                    \
			= dynamic_cast<ConstCharExpression *>((LHS).get());                \
		const auto rhs_char                                                    \
			= dynamic_cast<ConstCharExpression *>((RHS).get());                \
                                                                               \
		if (lhs_int)                                                           \
		{                                                                      \
			if (rhs_int)                                                       \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_int->Value O rhs_int->Value);                 \
			if (rhs_char)                                                      \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_int->Value O rhs_char->Value);                \
			return {};                                                         \
		}                                                                      \
		if (lhs_char)                                                          \
		{                                                                      \
			if (rhs_int)                                                       \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_char->Value O rhs_int->Value);                \
			if (rhs_char)                                                      \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_char->Value O rhs_char->Value);               \
			return {};                                                         \
		}                                                                      \
		return {};                                                             \
	} while (0)

#define COMPUTE_CMP(WHERE, LHS, O, RHS)                                        \
	do {                                                                       \
		const auto lhs_int = dynamic_cast<ConstIntExpression *>((LHS).get());  \
		const auto rhs_int = dynamic_cast<ConstIntExpression *>((RHS).get());  \
		const auto lhs_float                                                   \
			= dynamic_cast<ConstFloatExpression *>((LHS).get());               \
		const auto rhs_float                                                   \
			= dynamic_cast<ConstFloatExpression *>((RHS).get());               \
		const auto lhs_char                                                    \
			= dynamic_cast<ConstCharExpression *>((LHS).get());                \
		const auto rhs_char                                                    \
			= dynamic_cast<ConstCharExpression *>((RHS).get());                \
                                                                               \
		if (lhs_int)                                                           \
		{                                                                      \
			if (rhs_int)                                                       \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_int->Value O rhs_int->Value);                 \
			if (rhs_float)                                                     \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE),                                                   \
					static_cast<double>(lhs_int->Value) O rhs_float->Value);   \
			if (rhs_char)                                                      \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_int->Value O rhs_char->Value);                \
			return {};                                                         \
		}                                                                      \
		if (lhs_float)                                                         \
		{                                                                      \
			if (rhs_int)                                                       \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE),                                                   \
					lhs_float->Value O static_cast<double>(rhs_int->Value));   \
			if (rhs_float)                                                     \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_float->Value O rhs_float->Value);             \
			if (rhs_char)                                                      \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_float->Value O rhs_char->Value);              \
			return {};                                                         \
		}                                                                      \
		if (lhs_char)                                                          \
		{                                                                      \
			if (rhs_int)                                                       \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_char->Value O rhs_int->Value);                \
			if (rhs_float)                                                     \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_char->Value O rhs_float->Value);              \
			if (rhs_char)                                                      \
				return std::make_unique<ConstIntExpression>(                   \
					(WHERE), lhs_char->Value O rhs_char->Value);               \
			return {};                                                         \
		}                                                                      \
		return {};                                                             \
	} while (0)

QLang::ExpressionPtr QLang::BinaryExpression::Compress()
{
	if (auto lhs = LHS->Compress()) LHS = std::move(lhs);
	if (auto rhs = RHS->Compress()) RHS = std::move(rhs);
	if (!IsConstant()) return {};

	if (Operator == "+") COMPUTE_IFC(Where, LHS, +, RHS);
	if (Operator == "-") COMPUTE_IFC(Where, LHS, -, RHS);
	if (Operator == "*") COMPUTE_IFC(Where, LHS, *, RHS);
	if (Operator == "/") COMPUTE_IFC(Where, LHS, /, RHS);
	if (Operator == "%") COMPUTE_IC(Where, LHS, %, RHS);
	if (Operator == "&") COMPUTE_IC(Where, LHS, &, RHS);
	if (Operator == "|") COMPUTE_IC(Where, LHS, |, RHS);
	if (Operator == "^") COMPUTE_IC(Where, LHS, ^, RHS);
	if (Operator == "==") COMPUTE_CMP(Where, LHS, ==, RHS);
	if (Operator == "!=") COMPUTE_CMP(Where, LHS, !=, RHS);
	if (Operator == "<=") COMPUTE_CMP(Where, LHS, <=, RHS);
	if (Operator == ">=") COMPUTE_CMP(Where, LHS, >=, RHS);
	if (Operator == "<") COMPUTE_CMP(Where, LHS, <, RHS);
	if (Operator == ">") COMPUTE_CMP(Where, LHS, >, RHS);
	if (Operator == "&&") COMPUTE_IC(Where, LHS, &&, RHS);
	if (Operator == "||") COMPUTE_IC(Where, LHS, ||, RHS);
	if (Operator == "<<") COMPUTE_IC(Where, LHS, <<, RHS);
	if (Operator == ">>") COMPUTE_IC(Where, LHS, >>, RHS);

	return {};
}
