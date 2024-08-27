#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::ReturnStatement::ReturnStatement(
	const SourceLocation &where, ExpressionPtr value)
	: Statement(where), Value(std::move(value))
{
}

std::ostream &QLang::ReturnStatement::Print(std::ostream &stream) const
{
	if (!Value) return stream << "return void";
	return stream << "return " << Value;
}

void QLang::ReturnStatement::GenIRVoid(Builder &builder) const
{
	if (!Value)
	{
		builder.IRBuilder().CreateRetVoid();
		return;
	}

	auto value = Value->GenIR(builder);
	builder.IRBuilder().CreateRet(value->Get());
}
