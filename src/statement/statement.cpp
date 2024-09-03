#include <QLang/Statement.hpp>
#include <iostream>

QLang::Statement::Statement(const SourceLocation &where) : Where(where) {}

QLang::Statement::~Statement() = default;

std::ostream &QLang::operator<<(std::ostream &stream, const StatementPtr &ptr)
{
	if (!ptr) return stream << "(null)";
	return ptr->Print(stream);
}

void QLang::Print(const StatementPtr &ptr) { std::cout << ptr << std::endl; }
