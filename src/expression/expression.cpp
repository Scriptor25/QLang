#include <QLang/Expression.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::Expression::Expression(const SourceLocation& where) : Statement(where)
{
}

void QLang::Expression::GenIRVoid(Builder& builder) const { GenIR(builder); }

bool QLang::Expression::IsConstant() const { return false; }

std::ostream& QLang::operator<<(std::ostream& stream, const ExpressionPtr& ptr)
{
    if (!ptr) return stream << "(null)";
    return ptr->Print(stream);
}

QLang::ExpressionPtr QLang::Collapse(StatementPtr ptr)
{
    return Collapse(dyn_cast<Expression>(ptr));
}

QLang::ExpressionPtr QLang::Collapse(ExpressionPtr ptr)
{
    if (auto result = ptr->Collapse()) return result;
    return ptr;
}
