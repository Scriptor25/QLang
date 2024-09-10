#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Value.hpp>
#include <iostream>
#include <utility>

QLang::UnaryExpression::UnaryExpression(
    const SourceLocation& where, const std::string& operator_,
    StatementPtr operand, const bool post)
    : UnaryExpression(where, operator_, dyn_cast<Expression>(operand), post)
{
}

QLang::UnaryExpression::UnaryExpression(
    const SourceLocation& where, std::string operator_, ExpressionPtr operand,
    const bool post)
    : Expression(where), Operator(std::move(operator_)),
      Operand(std::move(operand)), Post(post)
{
}

bool QLang::UnaryExpression::IsConstant() const
{
    return (Operator == "!" || Operator == "-" || Operator == "~")
        && Operand->IsConstant();
}

std::ostream& QLang::UnaryExpression::Print(std::ostream& stream) const
{
    return stream << '(' << (Post ? "" : Operator) << Operand
        << (Post ? Operator : "") << ')';
}

QLang::ValuePtr QLang::UnaryExpression::GenIR(Builder& builder) const
{
    builder.SetLoc(Where);

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
        = builder.FindFunction(func_name, {}, {operand->GetType()}))
    {
        if (auto result
            = GenCall(builder, func->AsValue(builder), {}, {operand}))
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

QLang::ExpressionPtr QLang::UnaryExpression::Collapse()
{
    if (auto operand = Operand->Collapse()) Operand = std::move(operand);
    if (!IsConstant()) return {};

    const auto op_int = dynamic_cast<ConstIntExpression*>(Operand.get());
    const auto op_float = dynamic_cast<ConstFloatExpression*>(Operand.get());
    const auto op_char = dynamic_cast<ConstCharExpression*>(Operand.get());

    if (Operator == "!")
    {
        bool result;
        if (op_int) { result = op_int->Value == 0; }
        else if (op_float) { result = op_float->Value == 0; }
        else if (op_char) { result = op_char->Value == 0; }
        else { return {}; }
        return std::make_unique<ConstIntExpression>(Where, result);
    }
    if (Operator == "~")
    {
        uint64_t result;
        if (op_int) { result = ~op_int->Value; }
        else if (op_char) { result = ~op_char->Value; }
        else { return {}; }
        return std::make_unique<ConstIntExpression>(Where, result);
    }
    if (Operator == "-")
    {
        if (op_int)
        {
            return std::make_unique<ConstIntExpression>(Where, -op_int->Value);
        }
        if (op_float)
        {
            return std::make_unique<ConstFloatExpression>(
                Where, -op_float->Value);
        }
        if (op_char)
        {
            return std::make_unique<ConstCharExpression>(
                Where, -op_char->Value);
        }
        return {};
    }
    return {};
}
