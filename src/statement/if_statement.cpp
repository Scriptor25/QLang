#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Value.hpp>
#include <iostream>
#include <llvm/IR/BasicBlock.h>

QLang::IfStatement::IfStatement(
	const SourceLocation &where, StatementPtr if_, StatementPtr then,
	StatementPtr else_)
	: IfStatement(
		  where, dyn_cast<Expression>(if_), std::move(then), std::move(else_))
{
}

QLang::IfStatement::IfStatement(
	const SourceLocation &where, ExpressionPtr if_, StatementPtr then,
	StatementPtr else_)
	: Statement(where), If(std::move(if_)), Then(std::move(then)),
	  Else(std::move(else_))
{
}

std::ostream &QLang::IfStatement::Print(std::ostream &stream) const
{
	if (!Else) return stream << "if " << If << ' ' << Then;
	return stream << "if " << If << ' ' << Then << " else " << Else;
}

void QLang::IfStatement::GenIRVoid(Builder &builder) const
{
	const auto bkp = builder.IRBuilder().GetInsertBlock();
	const auto parent = bkp->getParent();
	auto then = llvm::BasicBlock::Create(builder.IRContext(), "then", parent);
	auto else_
		= Else ? llvm::BasicBlock::Create(builder.IRContext(), "else", parent)
			   : llvm::BasicBlock::Create(builder.IRContext(), "end", parent);
	const auto end
		= Else ? llvm::BasicBlock::Create(builder.IRContext(), "end", parent)
			   : else_;

	const auto if_ = If->GenIR(builder);
	if (!if_)
	{
		std::cerr << "    at " << Where << std::endl;

		builder.IRBuilder().SetInsertPoint(bkp);
		then->eraseFromParent();
		else_->eraseFromParent();
		if (Else) end->eraseFromParent();
		return;
	}

	const auto condition = builder.IRBuilder().CreateIsNotNull(if_->Get());
	builder.IRBuilder().CreateCondBr(condition, then, else_);

	builder.IRBuilder().SetInsertPoint(then);
	Then->GenIRVoid(builder);
	then = builder.IRBuilder().GetInsertBlock();
	if (!then->getTerminator()) builder.IRBuilder().CreateBr(end);

	if (Else)
	{
		builder.IRBuilder().SetInsertPoint(else_);
		Else->GenIRVoid(builder);
		else_ = builder.IRBuilder().GetInsertBlock();
		if (!else_->getTerminator()) builder.IRBuilder().CreateBr(end);
	}

	if (end->hasNPredecessors(0)) end->eraseFromParent();
	else
		builder.IRBuilder().SetInsertPoint(end);
}
