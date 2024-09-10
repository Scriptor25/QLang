#include <QLang/Builder.hpp>
#include <QLang/QLang.hpp>
#include <QLang/SourceLocation.hpp>

std::ostream& QLang::operator<<(std::ostream& stream, const SourceLocation& where)
{
    return stream << where.Filename << ":(" << where.Row << ',' << where.Column << ')';
}

llvm::DILocation* QLang::SourceLocation::GenDI(const Builder& builder) const
{
    return llvm::DILocation::get(builder.IRContext(), Row, Column, builder.Scope());
}
