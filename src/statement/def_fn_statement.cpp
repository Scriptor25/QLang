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
    std::vector<TypePtr> param_types;
    param_types.reserve(Params.size());
    for (const auto& [_type, _name] : Params) param_types.push_back(_type);
    const auto type = FunctionType::Get(Mode, Result, Self, param_types, VarArg);

    auto& [func_name, func_type, func_ir, func_ir_type] = builder.GetFunction(Name, type);
    if (!func_ir)
    {
        func_name = Name;
        func_type = type;
        func_ir_type = type->GenIR(builder);
        func_ir = llvm::Function::Create(func_ir_type, llvm::GlobalValue::ExternalLinkage, GenName(),
                                         builder.IRModule());
    }

    if (!Body) return;
    if (!func_ir->empty())
    {
        std::cerr << "at " << Where << ": cannot redefine function" << std::endl;
        return;
    }

    const auto bb = llvm::BasicBlock::Create(builder.IRContext(), "entry", func_ir);
    builder.IRBuilder().SetInsertPoint(bb);

    builder.StackPush();
    builder.GetResult() = type->GetResult();
    builder.ClearLocalDestructors();

    const unsigned off = Self ? 1 : 0;
    if (off)
    {
        const auto arg = func_ir->getArg(0);
        arg->setName("self");
        builder["self"] = LValue::Create(builder, Self, arg);
    }

    for (size_t i = 0; i < Params.size(); ++i)
    {
        auto& [_type, _name] = Params[i];

        const auto arg = func_ir->getArg(i + off);
        arg->setName(_name);

        if (const auto aty = ReferenceType::From(_type))
        {
            builder[_name] = LValue::Create(builder, aty->GetBase(), arg);
        }
        else { builder[_name] = LValue::Alloca(builder, _type, arg, _name); }
    }

    Body->GenIRVoid(builder);
    builder.StackPop();

    if (type->GetResult()->IsVoid())
        for (auto& block : *func_ir)
        {
            if (block.getTerminator()) continue;
            builder.IRBuilder().SetInsertPoint(&block);
            builder.IRBuilder().CreateRetVoid();
        }

    for (auto& block : *func_ir)
    {
        const auto terminator = block.getTerminator();
        if (!terminator || !llvm::dyn_cast<llvm::ReturnInst>(terminator)) continue;

        builder.IRBuilder().SetInsertPoint(terminator);
        builder.GenLocalDestructors();
    }

    builder.IRBuilder().ClearInsertionPoint();

    if (verifyFunction(*func_ir, &llvm::errs()))
    {
        func_ir->print(llvm::errs());
        func_ir->erase(func_ir->begin(), func_ir->end());
        return;
    }

    builder.Optimize(func_ir);
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
