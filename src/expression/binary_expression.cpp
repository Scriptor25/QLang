#include <iostream>
#include <memory>
#include <utility>
#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Value.hpp>

QLang::BinaryExpression::BinaryExpression(const SourceLocation& where,
                                          const std::string& operator_,
                                          StatementPtr lhs,
                                          StatementPtr rhs)
    : BinaryExpression(where, operator_, dyn_cast<Expression>(lhs), dyn_cast<Expression>(rhs))
{
}

QLang::BinaryExpression::BinaryExpression(const SourceLocation& where,
                                          std::string operator_,
                                          ExpressionPtr lhs,
                                          ExpressionPtr rhs)
    : Expression(where), Operator(std::move(operator_)), LHS(std::move(lhs)), RHS(std::move(rhs))
{
}

bool QLang::BinaryExpression::IsConstant() const
{
    return LHS->IsConstant() && RHS->IsConstant();
}

std::ostream& QLang::BinaryExpression::Print(std::ostream& stream) const
{
    if (Operator == "[]") return stream << LHS << '[' << RHS << ']';
    if (Operator == "." || Operator == "!") return stream << LHS << Operator << RHS;
    return stream << '(' << LHS << ' ' << Operator << ' ' << RHS << ')';
}

QLang::ValuePtr QLang::BinaryExpression::GenIR(Builder& builder) const
{
    const auto bkp = builder.IsCallee();
    builder.ClearCallee();

    auto lhs = LHS->GenIR(builder);
    if (!lhs) return {};

    if (bkp) builder.SetCallee();

    if (Operator == "." || Operator == "!")
    {
        const auto deref = Operator == "!";
        const auto member = dynamic_cast<const NameExpression*>(RHS.get());
        return GenMember(Where, builder, lhs, deref, member->Name);
    }

    auto rhs = RHS->GenIR(builder);
    if (!rhs) return {};

    auto self = LValue::From(lhs);
    if (self)
        if (const auto func = builder.FindFunction("operator" + Operator, lhs->GetType(), {rhs->GetType()}))
            return GenCall(Where, builder, func->AsValue(builder), self, {rhs});

    if (const auto func = builder.FindFunction("operator" + Operator, {}, {lhs->GetType(), rhs->GetType()}))
        return GenCall(Where, builder, func->AsValue(builder), {}, {lhs, rhs});

    if (Operator == "=")
    {
        if (!self)
        {
            std::cerr << "at " << Where << ": lhs must be a lvalue here" << std::endl;
            return {};
        }

        rhs = GenCast(Where, builder, rhs, self->GetType());
        if (!rhs) return {};

        self->Set(rhs->Get());
        return self;
    }

    if (Operator == "[]")
        return GenSubscript(Where, builder, lhs, rhs);

    std::string op = Operator;
    bool assign = false;
    ValuePtr result;

    if (!(op == "==" || op == "!=" || op == "<=" || op == ">=") && op.back() == '=')
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

    lhs = GenCast(Where, builder, lhs, higher);
    if (!lhs) return {};

    rhs = GenCast(Where, builder, rhs, higher);
    if (!rhs) return {};

    if (op == "+") result = GenAdd(Where, builder, lhs, rhs);
    else if (op == "-") result = GenSub(Where, builder, lhs, rhs);
    else if (op == "*") result = GenMul(Where, builder, lhs, rhs);
    else if (op == "/") result = GenDiv(Where, builder, lhs, rhs);
    else if (op == "%") result = GenRem(Where, builder, lhs, rhs);
    else if (op == "&") result = GenAnd(Where, builder, lhs, rhs);
    else if (op == "|") result = GenOr(Where, builder, lhs, rhs);
    else if (op == "^") result = GenXor(Where, builder, lhs, rhs);
    else if (op == "==") result = GenEQ(Where, builder, lhs, rhs);
    else if (op == "!=") result = GenNE(Where, builder, lhs, rhs);
    else if (op == "<=") result = GenLE(Where, builder, lhs, rhs);
    else if (op == ">=") result = GenGE(Where, builder, lhs, rhs);
    else if (op == "<") result = GenLT(Where, builder, lhs, rhs);
    else if (op == ">") result = GenGT(Where, builder, lhs, rhs);
    else if (op == "&&") result = GenLAnd(Where, builder, lhs, rhs);
    else if (op == "||") result = GenLOr(Where, builder, lhs, rhs);
    else if (op == "^^") result = GenLXor(Where, builder, lhs, rhs);
    else if (op == "<<") result = GenShl(Where, builder, lhs, rhs);
    else if (op == ">>") result = GenLShr(Where, builder, lhs, rhs);
    else if (op == ">>>") result = GenAShr(Where, builder, lhs, rhs);

    if (!result)
    {
        std::cerr
            << "at "
            << Where
            << ": no such binary operator '"
            << lhs->GetType()
            << ' '
            << Operator
            << ' '
            << rhs->GetType()
            << "'"
            << std::endl;
        return {};
    }

    if (assign)
    {
        if (!self)
        {
            std::cerr << "at " << Where << ": lhs must be a lvalue here" << std::endl;
            return {};
        }

        result = GenCast(Where, builder, result, self->GetType());
        if (!result) return {};

        self->Set(result->Get());
        return self;
    }

    return result;
}

#define COMPUTE_IFC(WHERE, LHS, O, RHS)                                                                                        \
	do {                                                                                                                       \
		const auto lhs_int = dynamic_cast<ConstIntExpression *>((LHS).get());                                                  \
		const auto rhs_int = dynamic_cast<ConstIntExpression *>((RHS).get());                                                  \
		const auto lhs_float = dynamic_cast<ConstFloatExpression *>((LHS).get());                                              \
		const auto rhs_float = dynamic_cast<ConstFloatExpression *>((RHS).get());                                              \
		const auto lhs_char = dynamic_cast<ConstCharExpression *>((LHS).get());                                                \
		const auto rhs_char = dynamic_cast<ConstCharExpression *>((RHS).get());                                                \
                                                                                                                               \
		if (lhs_int)                                                                                                           \
		{                                                                                                                      \
			if (rhs_int)                                                                                                       \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_int->Value O rhs_int->Value);                         \
			if (rhs_float)                                                                                                     \
			return std::make_unique<ConstFloatExpression>((WHERE), static_cast<double>(lhs_int->Value) O rhs_float->Value);    \
			if (rhs_char)                                                                                                      \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_int->Value O rhs_char->Value);                        \
			return {};                                                                                                         \
		}                                                                                                                      \
		if (lhs_float)                                                                                                         \
		{                                                                                                                      \
			if (rhs_int)                                                                                                       \
				return std::make_unique<ConstFloatExpression>((WHERE), lhs_float->Value O static_cast<double>(rhs_int->Value));\
			if (rhs_float)                                                                                                     \
				return std::make_unique<ConstFloatExpression>((WHERE), lhs_float->Value O rhs_float->Value);                   \
			if (rhs_char)                                                                                                      \
				return std::make_unique<ConstFloatExpression>((WHERE), lhs_float->Value O rhs_char->Value);                    \
			return {};                                                                                                         \
		}                                                                                                                      \
		if (lhs_char)                                                                                                          \
		{                                                                                                                      \
			if (rhs_int)                                                                                                       \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_char->Value O rhs_int->Value);                        \
			if (rhs_float)                                                                                                     \
				return std::make_unique<ConstFloatExpression>((WHERE), lhs_char->Value O rhs_float->Value);                    \
			if (rhs_char)                                                                                                      \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_char->Value O rhs_char->Value);                       \
			return {};                                                                                                         \
		}                                                                                                                      \
		return {};                                                                                                             \
	} while (0)

#define COMPUTE_IC(WHERE, LHS, O, RHS)                                                                  \
	do {                                                                                                \
		const auto lhs_int = dynamic_cast<ConstIntExpression *>((LHS).get());                           \
		const auto rhs_int = dynamic_cast<ConstIntExpression *>((RHS).get());                           \
		const auto lhs_char = dynamic_cast<ConstCharExpression *>((LHS).get());                         \
		const auto rhs_char = dynamic_cast<ConstCharExpression *>((RHS).get());                         \
                                                                                                        \
		if (lhs_int)                                                                                    \
		{                                                                                               \
			if (rhs_int)                                                                                \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_int->Value O rhs_int->Value);  \
			if (rhs_char)                                                                               \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_int->Value O rhs_char->Value); \
			return {};                                                                                  \
		}                                                                                               \
		if (lhs_char)                                                                                   \
		{                                                                                               \
			if (rhs_int)                                                                                \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_char->Value O rhs_int->Value); \
			if (rhs_char)                                                                               \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_char->Value O rhs_char->Value);\
			return {};                                                                                  \
		}                                                                                               \
		return {};                                                                                      \
	} while (0)

#define COMPUTE_CMP(WHERE, LHS, O, RHS)                                                                                      \
	do {                                                                                                                     \
		const auto lhs_int = dynamic_cast<ConstIntExpression *>((LHS).get());                                                \
		const auto rhs_int = dynamic_cast<ConstIntExpression *>((RHS).get());                                                \
		const auto lhs_float = dynamic_cast<ConstFloatExpression *>((LHS).get());                                            \
		const auto rhs_float = dynamic_cast<ConstFloatExpression *>((RHS).get());                                            \
		const auto lhs_char = dynamic_cast<ConstCharExpression *>((LHS).get());                                              \
		const auto rhs_char = dynamic_cast<ConstCharExpression *>((RHS).get());                                              \
                                                                                                                             \
		if (lhs_int)                                                                                                         \
		{                                                                                                                    \
			if (rhs_int)                                                                                                     \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_int->Value O rhs_int->Value);                       \
			if (rhs_float)                                                                                                   \
				return std::make_unique<ConstIntExpression>((WHERE), static_cast<double>(lhs_int->Value) O rhs_float->Value);\
			if (rhs_char)                                                                                                    \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_int->Value O rhs_char->Value);                      \
			return {};                                                                                                       \
		}                                                                                                                    \
		if (lhs_float)                                                                                                       \
		{                                                                                                                    \
			if (rhs_int)                                                                                                     \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_float->Value O static_cast<double>(rhs_int->Value));\
			if (rhs_float)                                                                                                   \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_float->Value O rhs_float->Value);                   \
			if (rhs_char)                                                                                                    \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_float->Value O rhs_char->Value);                    \
			return {};                                                                                                       \
		}                                                                                                                    \
		if (lhs_char)                                                                                                        \
		{                                                                                                                    \
			if (rhs_int)                                                                                                     \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_char->Value O rhs_int->Value);                      \
			if (rhs_float)                                                                                                   \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_char->Value O rhs_float->Value);                    \
			if (rhs_char)                                                                                                    \
				return std::make_unique<ConstIntExpression>((WHERE), lhs_char->Value O rhs_char->Value);                     \
			return {};                                                                                                       \
		}                                                                                                                    \
		return {};                                                                                                           \
	} while (0)

QLang::ExpressionPtr QLang::BinaryExpression::Collapse()
{
    if (auto lhs = LHS->Collapse()) LHS = std::move(lhs);
    if (auto rhs = RHS->Collapse()) RHS = std::move(rhs);
    if (!IsConstant()) return {};

    if (Operator == "+")
        COMPUTE_IFC(Where, LHS, +, RHS);
    if (Operator == "-")
        COMPUTE_IFC(Where, LHS, -, RHS);
    if (Operator == "*")
        COMPUTE_IFC(Where, LHS, *, RHS);
    if (Operator == "/")
        COMPUTE_IFC(Where, LHS, /, RHS);
    if (Operator == "%")
        COMPUTE_IC(Where, LHS, %, RHS);
    if (Operator == "&")
        COMPUTE_IC(Where, LHS, &, RHS);
    if (Operator == "|")
        COMPUTE_IC(Where, LHS, |, RHS);
    if (Operator == "^")
        COMPUTE_IC(Where, LHS, ^, RHS);
    if (Operator == "==")
        COMPUTE_CMP(Where, LHS, ==, RHS);
    if (Operator == "!=")
        COMPUTE_CMP(Where, LHS, !=, RHS);
    if (Operator == "<=")
        COMPUTE_CMP(Where, LHS, <=, RHS);
    if (Operator == ">=")
        COMPUTE_CMP(Where, LHS, >=, RHS);
    if (Operator == "<")
        COMPUTE_CMP(Where, LHS, <, RHS);
    if (Operator == ">")
        COMPUTE_CMP(Where, LHS, >, RHS);
    if (Operator == "&&")
        COMPUTE_IC(Where, LHS, &&, RHS);
    if (Operator == "||")
        COMPUTE_IC(Where, LHS, ||, RHS);
    if (Operator == "<<")
        COMPUTE_IC(Where, LHS, <<, RHS);
    if (Operator == ">>")
        COMPUTE_IC(Where, LHS, >>, RHS);

    return {};
}
