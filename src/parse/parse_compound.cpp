#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>
#include <iostream>
#include <vector>

QLang::StatementPtr QLang::Parser::ParseCompound()
{
	auto where = Expect("{").Where;

	std::vector<StatementPtr> list;
	while (!NextIfAt("}"))
	{
		auto ptr = ParseStatement();
		if (!ptr)
		{
			std::cerr << "wtf" << std::endl;
			continue;
		}
		list.push_back(std::move(ptr));
	}

	return std::make_unique<CompoundStatement>(where, list);
}
