#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::TernaryExpression::TernaryExpression(
    const SourceLocation& where, StatementPtr if_, StatementPtr then,
    StatementPtr else_)
    : TernaryExpression(
        where, dyn_cast<Expression>(std::move(if_)),
        dyn_cast<Expression>(std::move(then)),
        dyn_cast<Expression>(std::move(else_)))
{
}

QLang::TernaryExpression::TernaryExpression(
    const SourceLocation& where, ExpressionPtr if_, ExpressionPtr then,
    ExpressionPtr else_)
    : Expression(where), If(std::move(if_)), Then(std::move(then)),
      Else(std::move(else_))
{
}

bool QLang::TernaryExpression::IsConstant() const
{
    if (!If->IsConstant()) return false;
    return Then->IsConstant() || Else->IsConstant();
}

std::ostream& QLang::TernaryExpression::Print(std::ostream& stream) const
{
    return stream << '(' << If << " ? " << Then << " : " << Else << ')';
}

QLang::ValuePtr QLang::TernaryExpression::GenIR(Builder& builder) const
{
    const auto bkp = builder.IRBuilder().GetInsertBlock();
    const auto parent = bkp->getParent();
    auto then = llvm::BasicBlock::Create(builder.IRContext(), "then", parent);
    auto else_ = llvm::BasicBlock::Create(builder.IRContext(), "else", parent);
    const auto end = llvm::BasicBlock::Create(builder.IRContext(), "end", parent);

    const auto if_ = If->GenIR(builder);
    if (!if_)
    {
        std::cerr << "    at " << Where << std::endl;
        builder.IRBuilder().SetInsertPoint(bkp);
        then->eraseFromParent();
        else_->eraseFromParent();
        end->eraseFromParent();
        return {};
    }

    const auto condition = builder.IRBuilder().CreateIsNotNull(if_->Get());
    const auto br = builder.IRBuilder().CreateCondBr(condition, then, else_);

    builder.IRBuilder().SetInsertPoint(then);
    auto then_value = Then->GenIR(builder);
    if (!then_value)
    {
        std::cerr << "    at " << Where << std::endl;
        builder.IRBuilder().SetInsertPoint(bkp);
        br->eraseFromParent();
        then->eraseFromParent();
        else_->eraseFromParent();
        end->eraseFromParent();
        return {};
    }
    then = builder.IRBuilder().GetInsertBlock();

    builder.IRBuilder().SetInsertPoint(else_);
    auto else_value = Else->GenIR(builder);
    if (!else_value)
    {
        std::cerr << "    at " << Where << std::endl;
        builder.IRBuilder().SetInsertPoint(bkp);
        br->eraseFromParent();
        then->eraseFromParent();
        else_->eraseFromParent();
        end->eraseFromParent();
        return {};
    }
    else_ = builder.IRBuilder().GetInsertBlock();

    const auto type = Type::HigherOrder(then_value->GetType(), else_value->GetType());
    if (!type)
    {
        std::cerr << "    at " << Where << std::endl;
        builder.IRBuilder().SetInsertPoint(bkp);
        br->eraseFromParent();
        then->eraseFromParent();
        else_->eraseFromParent();
        end->eraseFromParent();
        return {};
    }

    builder.IRBuilder().SetInsertPoint(then);
    if (then_value->GetType() != type)
    {
        then_value = GenCast(builder, then_value, type);
        if (!then_value)
        {
            std::cerr << "    at " << Where << std::endl;
            builder.IRBuilder().SetInsertPoint(bkp);
            br->eraseFromParent();
            then->eraseFromParent();
            else_->eraseFromParent();
            end->eraseFromParent();
            return {};
        }
    }
    then_value = RValue::Create(builder, then_value->GetType(), then_value->Get());
    builder.IRBuilder().CreateBr(end);

    builder.IRBuilder().SetInsertPoint(else_);
    if (else_value->GetType() != type)
    {
        else_value = GenCast(builder, else_value, type);
        if (!else_value)
        {
            std::cerr << "    at " << Where << std::endl;
            builder.IRBuilder().SetInsertPoint(bkp);
            br->eraseFromParent();
            then->eraseFromParent();
            else_->eraseFromParent();
            end->eraseFromParent();
            return {};
        }
    }
    else_value = RValue::Create(builder, else_value->GetType(), else_value->Get());
    builder.IRBuilder().CreateBr(end);

    builder.IRBuilder().SetInsertPoint(end);
    const auto ir_type = type->GenIR(builder);
    const auto phi = builder.IRBuilder().CreatePHI(ir_type, 2);
    phi->addIncoming(then_value->Get(), then);
    phi->addIncoming(else_value->Get(), else_);
    return RValue::Create(builder, type, phi);
}

QLang::ExpressionPtr QLang::TernaryExpression::Collapse()
{
    if (auto if_ = If->Collapse()) If = std::move(if_);
    if (auto then = Then->Collapse()) Then = std::move(then);
    if (auto else_ = Else->Collapse()) Else = std::move(else_);
    if (!IsConstant()) return {};

    const auto if_char = dynamic_cast<ConstCharExpression*>(If.get());
    const auto if_float = dynamic_cast<ConstFloatExpression*>(If.get());
    const auto if_int = dynamic_cast<ConstIntExpression*>(If.get());
    const auto if_string = dynamic_cast<ConstStringExpression*>(If.get());

    int if_ = -1;
    if (if_char) if_ = if_char->Value != 0;
    if (if_float) if_ = if_float->Value != 0;
    if (if_int) if_ = if_int->Value != 0;
    if (if_string) if_ = 1;

    if (if_ < 0) return {};

    if (if_)
    {
        if (Then->IsConstant()) return std::move(Then);
        return {};
    }

    if (Else->IsConstant()) return std::move(Else);
    return {};
}
