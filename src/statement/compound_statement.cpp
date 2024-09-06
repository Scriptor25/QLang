#include <functional>
#include <QLang/Builder.hpp>
#include <QLang/Statement.hpp>

QLang::CompoundStatement::CompoundStatement(const SourceLocation& where, std::vector<StatementPtr>& list)
    : Statement(where), List(std::move(list))
{
}

std::ostream& QLang::CompoundStatement::Print(std::ostream& stream) const
{
    static size_t depth = 0;
    static std::function indent = []
    {
        std::string str;
        for (size_t i = 0; i < depth; ++i) str += ' ';
        return str;
    };

    stream << '{';
    depth += 4;
    for (const auto& ptr : List) stream << std::endl << indent() << ptr;
    depth -= 4;
    return stream << std::endl << indent() << '}';
}

void QLang::CompoundStatement::GenIRVoid(Builder& builder) const
{
    builder.StackPush();
    for (const auto& ptr : List) ptr->GenIRVoid(builder);
    builder.StackPop();
}
