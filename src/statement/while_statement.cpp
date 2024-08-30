#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::WhileStatement::WhileStatement(
	const SourceLocation &where, ExpressionPtr while_, StatementPtr loop)
	: Statement(where), While(std::move(while_)), Loop(std::move(loop))
{
}

std::ostream &QLang::WhileStatement::Print(std::ostream &stream) const
{
	return stream << "while " << While << ' ' << Loop;
}

void QLang::WhileStatement::GenIRVoid(Builder &builder) const
{
	auto bkp = builder.IRBuilder().GetInsertBlock();
	auto parent = bkp->getParent();
	auto head = llvm::BasicBlock::Create(builder.IRContext(), "head", parent);
	auto loop = llvm::BasicBlock::Create(builder.IRContext(), "loop", parent);
	auto end = llvm::BasicBlock::Create(builder.IRContext(), "end", parent);

	auto br = builder.IRBuilder().CreateBr(head);

	builder.IRBuilder().SetInsertPoint(head);
	auto while_ = While->GenIR(builder);
	if (!while_)
	{
		std::cerr << "    at " << Where << std::endl;

		builder.IRBuilder().SetInsertPoint(bkp);
		br->eraseFromParent();
		head->eraseFromParent();
		loop->eraseFromParent();
		end->eraseFromParent();
		return;
	}

	auto condition = builder.IRBuilder().CreateIsNotNull(while_->Get());
	builder.IRBuilder().CreateCondBr(condition, loop, end);

	builder.IRBuilder().SetInsertPoint(loop);
	Loop->GenIRVoid(builder);
	builder.IRBuilder().CreateBr(head);

	builder.IRBuilder().SetInsertPoint(end);
}
