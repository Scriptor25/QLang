#include <iostream>
#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Value.hpp>

QLang::ReturnStatement::ReturnStatement(const SourceLocation& where, StatementPtr value)
    : ReturnStatement(where, dyn_cast<Expression>(value))
{
}

QLang::ReturnStatement::ReturnStatement(const SourceLocation& where, ExpressionPtr value)
    : Statement(where), Value(std::move(value))
{
}

std::ostream& QLang::ReturnStatement::Print(std::ostream& stream) const
{
    if (!Value) return stream << "return void";
    return stream << "return " << Value;
}

void QLang::ReturnStatement::GenIRVoid(Builder& builder) const
{
    if (!Value)
    {
        builder.IRBuilder().CreateRetVoid();
        return;
    }

    auto value = Value->GenIR(builder);
    if (!value) return;

    if (builder.GetResult()->IsReference())
    {
        if (const auto ref = LValue::From(value))
        {
            builder.IRBuilder().CreateRet(ref->GetPtr());
            return;
        }

        std::cerr << "at " << Where << ": result must be a lvalue here" << std::endl;
        return;
    }

    value = GenCast(Where, builder, value, builder.GetResult());
    if (!value) return;

    builder.RemoveLocalDtor(value);
    builder.IRBuilder().CreateRet(value->Get());
}
