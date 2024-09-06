#include <iostream>
#include <utility>
#include <QLang/Statement.hpp>

QLang::Statement::Statement(SourceLocation where)
    : Where(std::move(where))
{
}

QLang::Statement::~Statement() = default;

std::ostream& QLang::operator<<(std::ostream& stream, const StatementPtr& ptr)
{
    if (!ptr) return stream << "(null)";
    return ptr->Print(stream);
}

void QLang::Print(const StatementPtr& ptr) { std::cout << ptr << std::endl; }
