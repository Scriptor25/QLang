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
    const auto func = name.empty() ? new Function() : GetFunction(name, type);

    if (!func->IR)
    {
        func->Name = name;
        func->Type = type;
        func->IRType = type->GenIR(*this);
        func->IR = llvm::Function::Create(func->IRType, llvm::GlobalValue::ExternalLinkage, ir_name, IRModule());
    }

    if (!body)
        return func;

    if (!func->IR->empty())
    {
        std::cerr << "at " << where << ": cannot redefine function" << std::endl;
        return {};
    }

    const auto bkp = IRBuilder().GetInsertBlock();
    IRBuilder().SetInsertPoint(llvm::BasicBlock::Create(IRContext(), "entry", func->IR));

    const auto sp = m_DIBuilder->createFunction(
        m_CU,
        ir_name,
        name,
        m_CU->getFile(),
        where.Row,
        func->Type->GenDI(*this),
        where.Row,
        llvm::DINode::FlagPrototyped,
        llvm::DISubprogram::SPFlagDefinition);
    func->IR->setSubprogram(sp);

    m_IRBuilder->SetCurrentDebugLocation({});

    StackPush(name.empty());
    m_Frame.Scope = sp;
    GetResult() = result;
    ClearLocalDestructors();

    const unsigned off = self ? 1 : 0;
    if (off)
    {
        const auto arg = func->IR->getArg(0);
        arg->setName("self");
        const auto s = LValue::Create(*this, self, arg);
        (*this)["self"] = s;

        const auto d = m_DIBuilder->createParameterVariable(
            sp,
            "self",
            1,
            m_CU->getFile(),
            where.Row,
            self->GenDI(*this),
            true);
        m_DIBuilder->insertDeclare(
            s->GetPtr(),
            d,
            m_DIBuilder->createExpression(),
            where.GenDI(*this),
            m_IRBuilder->GetInsertBlock());
    }

    for (size_t i = 0; i < params.size(); ++i)
    {
        auto& [type_, name_] = params[i];
        if (name_.empty()) continue;

        const auto arg = func->IR->getArg(i + off);
        arg->setName(name_);

        LValuePtr val;
        if (const auto aty = ReferenceType::From(type_))
            val = LValue::Create(*this, aty->GetBase(), arg);
        else
            val = LValue::Alloca(*this, type_, arg, name_);
        (*this)[name_] = val;

        const auto d = m_DIBuilder->createParameterVariable(
            sp,
            name_,
            i + off + 1,
            m_CU->getFile(),
            where.Row,
            type_->GenDI(*this),
            true);
        m_DIBuilder->insertDeclare(
            val->GetPtr(),
            d,
            m_DIBuilder->createExpression(),
            where.GenDI(*this),
            m_IRBuilder->GetInsertBlock());
    }

    body->GenIRVoid(*this);

    if (result->IsVoid())
        for (auto& block : *func->IR)
        {
            if (block.getTerminator()) continue;
            IRBuilder().SetInsertPoint(&block);
            IRBuilder().CreateRetVoid();
        }

    for (auto& block : *func->IR)
    {
        const auto terminator = block.getTerminator();
        if (!terminator || !llvm::dyn_cast<llvm::ReturnInst>(terminator)) continue;

        IRBuilder().SetInsertPoint(terminator);
        GenLocalDestructors(where);
    }

    StackPop();
    IRBuilder().SetInsertPoint(bkp);

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
