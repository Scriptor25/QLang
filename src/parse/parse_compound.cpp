#include <vector>
#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>

QLang::StatementPtr QLang::Parser::ParseCompound()
{
    auto where = Expect("{").Where;

    std::vector<StatementPtr> list;
    while (!NextIfAt("}"))
    {
        auto ptr = ParseStatement();
        list.push_back(std::move(ptr));
    }

    return std::make_unique<CompoundStatement>(where, list);
}
