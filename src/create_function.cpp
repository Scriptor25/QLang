#include <iostream>
#include <llvm/IR/Verifier.h>
#include <QLang/Builder.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::Function* QLang::Builder::CreateFunction(const FnMode mode,
                                                const TypePtr& result,
                                                const TypePtr& self,
                                                const std::string& name,
                                                const std::string& ir_name,
                                                const std::vector<Param>& params,
                                                const bool vararg, const Statement* body)
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
        std::cerr << "cannot redefine function" << std::endl;
        return {};
    }

    const auto bkp = IRBuilder().GetInsertBlock();
    IRBuilder().SetInsertPoint(llvm::BasicBlock::Create(IRContext(), "entry", func->IR));

    StackPush(name.empty());
    GetResult() = result;
    ClearLocalDestructors();

    const unsigned off = self ? 1 : 0;
    if (off)
    {
        const auto arg = func->IR->getArg(0);
        arg->setName("self");
        (*this)["self"] = LValue::Create(*this, self, arg);
    }

    for (size_t i = 0; i < params.size(); ++i)
    {
        auto& [_type, _name] = params[i];
        if (_name.empty()) continue;

        const auto arg = func->IR->getArg(i + off);
        arg->setName(_name);

        if (const auto aty = ReferenceType::From(_type))
        {
            (*this)[_name] = LValue::Create(*this, aty->GetBase(), arg);
        }
        else { (*this)[_name] = LValue::Alloca(*this, _type, arg, _name); }
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
        GenLocalDestructors();
    }

    StackPop();
    IRBuilder().SetInsertPoint(bkp);

    if (verifyFunction(*func->IR, &llvm::errs()))
    {
        func->IR->print(llvm::errs());
        func->IR->erase(func->IR->begin(), func->IR->end());
        return {};
    }

    m_FPM->run(*func->IR, *m_FAM);
    return func;
}
