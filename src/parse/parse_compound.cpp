#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>
#include <vector>

QLang::StatementPtr QLang::Parser::ParseCompound()
{
	auto where = Expect("{").Where;

	std::vector<StatementPtr> list;
	while (!NextIfAt("}")) { list.push_back(Parse()); }

	return std::make_unique<CompoundStatement>(where, list);
}
