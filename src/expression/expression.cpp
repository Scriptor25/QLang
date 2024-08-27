#include <QLang/Expression.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Value.hpp>
#include <iostream>

QLang::Expression::Expression(const SourceLocation &where) : Statement(where) {}

void QLang::Expression::GenIRVoid(Builder &builder) const { GenIR(builder); }

std::ostream &QLang::operator<<(std::ostream &stream, const ExpressionPtr &ptr)
{
	if (!ptr) return stream << "(null)";
	return ptr->Print(stream);
}
