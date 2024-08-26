#include <QLang/Statement.hpp>

QLang::Statement::Statement(const SourceLocation &where) : Where(where) {}

QLang::Statement::~Statement() = default;
