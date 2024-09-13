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
    const auto scope = builder.DIBuilder().createLexicalBlock(
        builder.Scope(),
        builder.Scope()->getFile(),
        Where.Row,
        Where.Column);
    builder.Scope() = scope;
    for (const auto& ptr : List) ptr->GenIRVoid(builder);

    if (const auto terminator = builder.IRBuilder().GetInsertBlock()->getTerminator())
        builder.IRBuilder().SetInsertPoint(terminator);
    builder.GenLocalDestructors(Where);

    builder.StackPop();
}
