#include <iostream>
#include <string>
#include <utility>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <QLang/Builder.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::DefFnStatement::DefFnStatement(const SourceLocation& where,
                                      const bool is_extern,
                                      const FnMode mode,
                                      TypePtr result,
                                      TypePtr self,
                                      std::string name,
                                      const std::vector<Param>& params,
                                      const bool vararg,
                                      StatementPtr body)
    : Statement(where),
      IsExtern(is_extern),
      Mode(mode),
      Result(std::move(result)),
      Self(std::move(self)),
      Name(std::move(name)),
      Params(params),
      VarArg(vararg),
      Body(std::move(body))
{
}

std::ostream& QLang::DefFnStatement::Print(std::ostream& stream) const
{
    stream << "def ";
    if (IsExtern) stream << "ext ";
    if (Mode == FnMode_Func)
    {
        stream << Result << ' ';
        if (Self) stream << Self << ':';
    }
    else if (Mode == FnMode_Ctor) { stream << '+'; }
    else if (Mode == FnMode_Dtor) { stream << '-'; }
    stream << Name << '(';
    for (size_t i = 0; i < Params.size(); ++i)
    {
        if (i > 0) stream << ", ";
        stream << Params[i].Type;
        if (!Params[i].Name.empty()) stream << ' ' << Params[i].Name;
    }
    if (VarArg)
    {
        if (!Params.empty()) stream << ", ";
        stream << '?';
    }
    stream << ')';
    if (!Body) return stream;
    return stream << ' ' << Body;
}

void QLang::DefFnStatement::GenIRVoid(Builder& builder) const
{
    if (builder.CreateFunction(Mode, Result, Self, Name, GenName(), Params, VarArg, Body.get()))
        return;

    std::cerr << "    at " << Where << std::endl;
}

std::string QLang::DefFnStatement::GenName() const
{
    if (IsExtern) return Name;

    std::string name = Name;

    name += ';' + Result->GetName();

    if (Self)
    {
        name += ';';
        switch (Mode)
        {
        case FnMode_Func: name += ':';
            break;
        case FnMode_Ctor: name += '+';
            break;
        case FnMode_Dtor: name += '-';
            break;
        }
        name += Self->GetName();
    }

    name += ';' + std::to_string(Params.size());
    for (const auto& [param_type, param_name] : Params)
    {
        name += ';';
        name += param_type->GetName();
    }
    if (VarArg) name += ";?";

    const auto hash = std::hash<std::string>()(name);

    char buf[256];
    sprintf(buf, "%s__%016lX", Name.c_str(), hash);
    return buf;
}
