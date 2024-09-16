#include <memory>
#include <llvm/IR/Value.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/AggressiveInstCombine/AggressiveInstCombine.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/DCE.h>
#include <llvm/Transforms/Scalar/DeadStoreElimination.h>
#include <llvm/Transforms/Scalar/EarlyCSE.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/JumpThreading.h>
#include <llvm/Transforms/Scalar/LoopUnrollPass.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SCCP.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Utils/Mem2Reg.h>
#include <llvm/Transforms/Vectorize/LoadStoreVectorizer.h>
#include <llvm/Transforms/Vectorize/LoopVectorize.h>
#include <llvm/Transforms/Vectorize/SLPVectorizer.h>
#include <llvm/Transforms/Vectorize/VectorCombine.h>
#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::Builder::Builder(
    Context& context,
    llvm::LLVMContext& ir_context,
    const std::string& module_name,
    const std::string& filename,
    const std::string& directory,
    const bool debug,
    const unsigned optimization)
    : m_Context(context), m_IRContext(ir_context), m_Debug(debug), m_Optimization(optimization)
{
    m_IRModule = std::make_unique<llvm::Module>(module_name, m_IRContext);
    m_IRBuilder = std::make_unique<llvm::IRBuilder<>>(m_IRContext);
    m_DIBuilder = std::make_unique<llvm::DIBuilder>(*m_IRModule);

    m_Frame.Name = "GLOBAL";

    if (debug)
    {
        m_CU = m_DIBuilder->createCompileUnit(
            llvm::dwarf::DW_LANG_C_plus_plus,
            m_DIBuilder->createFile(filename, directory),
            "QLang",
            optimization,
            "",
            0);
        m_Frame.Scope = m_CU;
    }

    m_FPM = std::make_unique<llvm::FunctionPassManager>();
    m_LAM = std::make_unique<llvm::LoopAnalysisManager>();
    m_FAM = std::make_unique<llvm::FunctionAnalysisManager>();
    m_CGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
    m_MAM = std::make_unique<llvm::ModuleAnalysisManager>();
    m_PIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
    m_SI = std::make_unique<llvm::StandardInstrumentations>(m_IRContext, true);

    m_SI->registerCallbacks(*m_PIC, m_MAM.get());

    if (optimization >= 1)
    {
        m_FPM->addPass(llvm::DCEPass());
        m_FPM->addPass(llvm::DSEPass());
        m_FPM->addPass(llvm::JumpThreadingPass());
        m_FPM->addPass(llvm::EarlyCSEPass());
        m_FPM->addPass(llvm::SCCPPass());
        m_FPM->addPass(llvm::PromotePass());
        m_FPM->addPass(llvm::InstCombinePass());
        m_FPM->addPass(llvm::ReassociatePass());
        m_FPM->addPass(llvm::GVNPass());
        m_FPM->addPass(llvm::SimplifyCFGPass());
    }
    if (optimization >= 2)
    {
        m_FPM->addPass(llvm::LoopUnrollPass());
        m_FPM->addPass(llvm::LoadStoreVectorizerPass());
        m_FPM->addPass(llvm::LoopVectorizePass());
        m_FPM->addPass(llvm::SLPVectorizerPass());
        m_FPM->addPass(llvm::VectorCombinePass());
        m_FPM->addPass(llvm::AggressiveInstCombinePass());
    }

    llvm::PassBuilder pb;
    pb.registerModuleAnalyses(*m_MAM);
    pb.registerFunctionAnalyses(*m_FAM);
    pb.crossRegisterProxies(*m_LAM, *m_FAM, *m_CGAM, *m_MAM);
}

QLang::Context& QLang::Builder::GetContext() const { return m_Context; }

llvm::LLVMContext& QLang::Builder::IRContext() const { return m_IRContext; }

llvm::IRBuilder<>& QLang::Builder::IRBuilder() const { return *m_IRBuilder; }

llvm::DIBuilder& QLang::Builder::DIBuilder() const
{
    return *m_DIBuilder;
}

llvm::DIScope*& QLang::Builder::Scope()
{
    return m_Frame.Scope;
}

llvm::Module& QLang::Builder::IRModule() const { return *m_IRModule; }

std::unique_ptr<llvm::Module>& QLang::Builder::IRModulePtr()
{
    return m_IRModule;
}

bool QLang::Builder::Debug() const
{
    return m_Debug;
}

void QLang::Builder::SetLoc(const SourceLocation& where)
{
    if (m_Debug)
        IRBuilder().SetCurrentDebugLocation(where.GenDI(*this));
}

void QLang::Builder::Finalize() const
{
    if (m_Debug)
        m_DIBuilder->finalize();
}

void QLang::Builder::Print() const { m_IRModule->print(llvm::errs(), nullptr); }

void QLang::Builder::StackPush(const bool globalize)
{
    m_Stack.push_back(m_Frame);
    if (globalize)
        m_Frame = m_Stack.front();
}

void QLang::Builder::StackPop()
{
    m_Frame = m_Stack.back();
    m_Stack.pop_back();
}

QLang::ValuePtr& QLang::Builder::operator[](const std::string& name)
{
    return m_Frame.Values[name];
}

QLang::LValuePtr QLang::Builder::CreateInstance(
    const SourceLocation& where,
    const TypePtr& type,
    const bool init_null,
    const std::string& name)
{
    auto value = LValue::Alloca(*this, type, nullptr, name);

    if (m_Debug)
        DIDeclareVariable(where, type, name, value);

    const auto ir_type = type->GenIR(*this);

    if (init_null)
    {
        const auto null_value = llvm::Constant::getNullValue(ir_type);
        value->Set(null_value);
    }

    if (const auto struct_type = StructType::From(type))
    {
        for (size_t i = 0; i < struct_type->GetElementCount(); ++i)
        {
            const auto& [type_, name_, init_] = struct_type->GetElement(i);
            if (!init_) continue;

            auto init = init_->GenIR(*this);
            if (!init) return {};
            init = GenCast(where, *this, init, type_);
            if (!init) return {};

            const auto gep = m_IRBuilder->CreateStructGEP(ir_type, value->GetPtr(), i, name_);
            m_IRBuilder->CreateStore(init->Get(), gep);
        }
    }

    AddLocalDestructor(where, value);
    return value;
}

void QLang::Builder::ClearLocalDestructors() { m_Frame.LocalDestructors.clear(); }

void QLang::Builder::AddLocalDestructor(const SourceLocation& where, const LValuePtr& value)
{
    if (const auto func = FindDestructor(value->GetType()))
    {
        m_Frame.LocalDestructors.push_back({func, value});
        return;
    }

    if (const auto str_ty = StructType::From(value->GetType()))
        for (size_t i = 0; i < str_ty->GetElementCount(); ++i)
        {
            auto member = GenMember(where, *this, value, i);
            AddLocalDestructor(where, member);
        }
}

void QLang::Builder::RemoveLocalDtor(const ValuePtr& value)
{
    for (long i = 0; i < m_Frame.LocalDestructors.size(); ++i)
        if (m_Frame.LocalDestructors[i].Self == value)
        {
            m_Frame.LocalDestructors.erase(m_Frame.LocalDestructors.begin() + i);
            return;
        }
}

void QLang::Builder::GenLocalDestructors(const SourceLocation& where)
{
    for (auto& [callee_, self_] : m_Frame.LocalDestructors)
        GenCall(where, *this, callee_->AsValue(*this), self_, {});
    ClearLocalDestructors();
}

QLang::Function* QLang::Builder::GetFunction(const std::string& name, const FunctionTypePtr& type)
{
    return &m_Functions[name][type];
}

QLang::Function* QLang::Builder::FindFunction(
    const std::string& name,
    const TypePtr& self,
    const std::vector<TypePtr>& args)
{
    size_t low_error = -1;
    Function* func = nullptr;

    for (auto& [type_, func_] : m_Functions[name])
    {
        if (type_->GetSelf() != self) continue;
        if ((!type_->IsVarArg() && type_->GetParamCount() != args.size()) || type_->GetParamCount() > args.size())
            continue;

        size_t error = 0;
        for (size_t i = 0; i < type_->GetParamCount(); ++i)
        {
            const auto diff = Type::TypeDiff(*this, type_->GetParam(i), args[i]);
            if (diff == static_cast<size_t>(-1))
            {
                error = -1;
                break;
            }
            error += diff;
        }

        if (error < low_error)
        {
            low_error = error;
            func = &func_;
        }
    }

    return func;
}

QLang::Function* QLang::Builder::FindFunction(const std::string& name)
{
    auto& map = m_Functions[name];
    if (map.empty()) return {};
    return &map.begin()->second;
}

QLang::Function* QLang::Builder::FindConstructor(const TypePtr& self, const std::vector<TypePtr>& args)
{
    size_t low_error = -1;
    Function* func = nullptr;

    for (auto& [type_, func_] : m_Functions[self->GetName()])
    {
        if (type_->GetMode() != FnMode_Ctor) continue;
        if ((!type_->IsVarArg() && type_->GetParamCount() != args.size()) || type_->GetParamCount() > args.size())
            continue;

        size_t error = 0;
        for (size_t i = 0; i < type_->GetParamCount(); ++i)
            error += Type::TypeDiff(*this, type_->GetParam(i), args[i]);

        if (error < low_error)
        {
            low_error = error;
            func = &func_;
        }
    }

    return func;
}

QLang::Function* QLang::Builder::FindDestructor(const TypePtr& self)
{
    auto& func = m_Functions[self->GetName()][FunctionType::Get(FnMode_Dtor, GetVoidTy(), self, {}, false)];

    if (!func.IR) return nullptr;
    return &func;
}

QLang::TypePtr QLang::Builder::GetVoidTy() const
{
    return Type::Get(m_Context, "void");
}

QLang::TypePtr QLang::Builder::GetInt1Ty() const
{
    return Type::Get(m_Context, "i1");
}

QLang::TypePtr QLang::Builder::GetInt8Ty() const
{
    return Type::Get(m_Context, "i8");
}

QLang::TypePtr QLang::Builder::GetInt16Ty() const
{
    return Type::Get(m_Context, "i16");
}

QLang::TypePtr QLang::Builder::GetInt32Ty() const
{
    return Type::Get(m_Context, "i32");
}

QLang::TypePtr QLang::Builder::GetInt64Ty() const
{
    return Type::Get(m_Context, "i64");
}

QLang::TypePtr QLang::Builder::GetFloat16Ty() const
{
    return Type::Get(m_Context, "f16");
}

QLang::TypePtr QLang::Builder::GetFloat32Ty() const
{
    return Type::Get(m_Context, "f32");
}

QLang::TypePtr QLang::Builder::GetFloat64Ty() const
{
    return Type::Get(m_Context, "f64");
}

QLang::PointerTypePtr QLang::Builder::GetVoidPtrTy() const
{
    return PointerType::Get(GetVoidTy());
}

QLang::PointerTypePtr QLang::Builder::GetInt8PtrTy() const
{
    return PointerType::Get(GetInt8Ty());
}

bool QLang::Builder::IsCallee() const { return m_IsCallee; }

void QLang::Builder::SetCallee() { m_IsCallee = true; }

void QLang::Builder::ClearCallee() { m_IsCallee = false; }

QLang::ValuePtr& QLang::Builder::Self() { return m_Frame.Self; }

size_t QLang::Builder::GetArgCount() const { return m_Args.size(); }

QLang::TypePtr& QLang::Builder::GetArg(const size_t i) { return m_Args[i]; }

std::vector<QLang::TypePtr>& QLang::Builder::GetArgs() { return m_Args; }

QLang::TypePtr& QLang::Builder::GetResult() { return m_Frame.Result; }
