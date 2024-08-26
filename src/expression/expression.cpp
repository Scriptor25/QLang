#include <QLang/Expression.hpp>
#include <QLang/Statement.hpp>

QLang::Expression::Expression(const SourceLocation &where) : Statement(where) {}
