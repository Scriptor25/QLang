#include <iostream>
#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Value.hpp>

QLang::WhileStatement::WhileStatement(const SourceLocation& where, StatementPtr while_, StatementPtr loop)
    : WhileStatement(where, dyn_cast<Expression>(while_), std::move(loop))
{
}

QLang::WhileStatement::WhileStatement(const SourceLocation& where, ExpressionPtr while_, StatementPtr loop)
    : Statement(where), While(std::move(while_)), Loop(std::move(loop))
{
}

std::ostream& QLang::WhileStatement::Print(std::ostream& stream) const
{
    return stream << "while " << While << ' ' << Loop;
}

void QLang::WhileStatement::GenIRVoid(Builder& builder) const
{
    const auto bkp = builder.IRBuilder().GetInsertBlock();
    const auto parent = bkp->getParent();
    const auto head = llvm::BasicBlock::Create(builder.IRContext(), "head", parent);
    const auto loop = llvm::BasicBlock::Create(builder.IRContext(), "loop", parent);
    const auto end = llvm::BasicBlock::Create(builder.IRContext(), "end", parent);

    const auto br = builder.IRBuilder().CreateBr(head);

    builder.IRBuilder().SetInsertPoint(head);
    const auto while_ = While->GenIR(builder);
    if (!while_)
    {
        builder.IRBuilder().SetInsertPoint(bkp);
        br->eraseFromParent();
        head->eraseFromParent();
        loop->eraseFromParent();
        end->eraseFromParent();
        return;
    }

    const auto condition = builder.IRBuilder().CreateIsNotNull(while_->Get());
    builder.IRBuilder().CreateCondBr(condition, loop, end);

    builder.IRBuilder().SetInsertPoint(loop);
    Loop->GenIRVoid(builder);
    builder.IRBuilder().CreateBr(head);

    builder.IRBuilder().SetInsertPoint(end);
}
