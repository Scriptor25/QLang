#include <QLang/Statement.hpp>
#include <functional>
#include <iostream>

QLang::CompoundStatement::CompoundStatement(
	const SourceLocation &where, std::vector<StatementPtr> &list)
	: Statement(where)
{
	for (auto &ptr : list) List.push_back(std::move(ptr));
}

void QLang::CompoundStatement::Print(std::ostream &stream) const
{
	static size_t depth = 0;
	static std::function<std::string()> indent = []()
	{
		std::string str;
		for (size_t i = 0; i < depth; ++i) str += ' ';
		return str;
	};

	stream << '{';
	depth += 4;
	for (const auto &ptr : List)
	{
		stream << std::endl << indent();
		ptr->Print(stream);
	}
	depth -= 4;
	stream << std::endl << indent() << '}';
}
