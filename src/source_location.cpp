#include <QLang/QLang.hpp>
#include <QLang/SourceLocation.hpp>
#include <iostream>

std::ostream &QLang::operator<<(
	std::ostream &stream, const SourceLocation &where)
{
	return stream << where.Filename << '(' << where.Row << ';' << where.Column
				  << ')';
}
