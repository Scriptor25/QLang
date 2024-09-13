#include <iostream>
#include <llvm/IR/Verifier.h>
#include <QLang/Builder.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::Function* QLang::Builder::CreateFunction(
    const SourceLocation& where,
    const FnMode mode,
    const TypePtr& result,
    const TypePtr& self,
    const std::string& name,
    const std::string& ir_name,
    const std::vector<Param>& params,
    const bool vararg,
    const Statement* body)
{
    std::vector<TypePtr> param_types(params.size());
    for (size_t i = 0; i < params.size(); ++i)
        param_types[i] = params[i].Type;

    const auto type = FunctionType::Get(mode, result, self, param_types, vararg);

    Function* func;
    if (name.empty())
    {
        func = new Function();
        func->Name = m_Frame.Name + "$" + std::to_string(m_Frame.SubCount++);
    }
    else
        func = GetFunction(name, type);

    if (!func->IR)
    {
        if (!name.empty()) func->Name = name;
        func->Type = type;
        func->IRType = type->GenIR(*this);
        func->IR = llvm::Function::Create(func->IRType, llvm::GlobalValue::ExternalLinkage, ir_name, *m_IRModule);
    }

    if (!body)
        return func;

    if (!func->IR->empty())
    {
        std::cerr << "at " << where << ": cannot redefine function" << std::endl;
        return {};
    }

    const auto bkp = m_IRBuilder->GetInsertBlock();
    m_IRBuilder->SetInsertPoint(llvm::BasicBlock::Create(m_IRContext, "entry", func->IR));

    const auto sp = m_DIBuilder->createFunction(
        m_Frame.Scope,
        func->IR->getName(),
        func->Name,
        m_Frame.Scope->getFile(),
        where.Row,
        func->Type->GenDI(*this),
        where.Row,
        llvm::DINode::FlagPrototyped,
        llvm::DISubprogram::SPFlagDefinition);
    func->IR->setSubprogram(sp);

    m_IRBuilder->SetCurrentDebugLocation({});

    StackPush(true);
    m_Frame.Name = func->Name;
    m_Frame.SubCount = 0;
    m_Frame.Values.clear();
    m_Frame.LocalDestructors.clear();
    m_Frame.Self = {};
    m_Frame.Result = result;
    m_Frame.Scope = sp;

    const unsigned off = self ? 1 : 0;
    if (self)
    {
        const auto arg = func->IR->getArg(0);
        arg->setName("self");
        const auto value = LValue::Create(*this, self, arg);
        m_Frame.Values["self"] = value;

        DIDeclareParam(where, 1, self, "self", value);
    }

    for (size_t i = 0; i < params.size(); ++i)
    {
        auto& [type_, name_] = params[i];
        if (name_.empty()) continue;

        const auto arg = func->IR->getArg(i + off);
        arg->setName(name_);

        LValuePtr value;
        if (const auto aty = ReferenceType::From(type_))
            value = LValue::Create(*this, aty->GetBase(), arg);
        else
            value = LValue::Alloca(*this, type_, arg, name_);
        m_Frame.Values[name_] = value;

        DIDeclareParam(where, i + off + 1, type_, name_, value);
    }

    body->GenIRVoid(*this);

    if (result->IsVoid())
        for (auto& block : *func->IR)
        {
            if (block.getTerminator()) continue;
            m_IRBuilder->SetInsertPoint(&block);
            m_IRBuilder->CreateRetVoid();
        }

    for (auto& block : *func->IR)
    {
        const auto terminator = block.getTerminator();
        if (!terminator || !llvm::dyn_cast<llvm::ReturnInst>(terminator)) continue;

        m_IRBuilder->SetInsertPoint(terminator);
        GenLocalDestructors(where);
    }

    StackPop();
    m_IRBuilder->SetInsertPoint(bkp);

    if (verifyFunction(*func->IR, &llvm::errs()))
    {
        func->IR->print(llvm::errs());
        func->IR->erase(func->IR->begin(), func->IR->end());
        std::cerr << "at " << where << ": failed to verify function" << std::endl;
        return {};
    }

    m_FPM->run(*func->IR, *m_FAM);
    return func;
}

void QLang::Builder::DIDeclareParam(
    const SourceLocation& where,
    const size_t index,
    const TypePtr& type,
    const std::string& name,
    const LValuePtr& value)
{
    const auto param = m_DIBuilder->createParameterVariable(
        m_Frame.Scope,
        name,
        index,
        m_Frame.Scope->getFile(),
        where.Row,
        type->GenDI(*this),
        true);
    m_DIBuilder->insertDeclare(
        value->GetPtr(),
        param,
        m_DIBuilder->createExpression(),
        where.GenDI(*this),
        m_IRBuilder->GetInsertBlock());
}
