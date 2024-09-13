#pragma once

#include <map>
#include <memory>
#include <string>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <QLang/Function.hpp>
#include <QLang/QLang.hpp>

namespace QLang
{
    struct DtorCall
    {
        Function* Callee = nullptr;
        LValuePtr Self;
    };

    struct StackFrame
    {
        std::string Name;
        size_t SubCount = 0;

        std::map<std::string, ValuePtr> Values;
        std::vector<DtorCall> LocalDestructors;

        ValuePtr Self;
        TypePtr Result;

        llvm::DIScope* Scope = nullptr;
    };

    class Builder
    {
    public:
        Builder(
            Context&,
            llvm::LLVMContext&,
            const std::string& module_name,
            const std::string& filename,
            const std::string& directory,
            bool debug,
            unsigned optimization);

        [[nodiscard]] Context& GetContext() const;

        [[nodiscard]] llvm::LLVMContext& IRContext() const;
        [[nodiscard]] llvm::Module& IRModule() const;
        [[nodiscard]] llvm::IRBuilder<>& IRBuilder() const;
        [[nodiscard]] llvm::DIBuilder& DIBuilder() const;

        [[nodiscard]] llvm::DIScope*& Scope();

        std::unique_ptr<llvm::Module>& IRModulePtr();

        [[nodiscard]] bool Debug() const;
        void SetLoc(const SourceLocation&);

        void Finalize() const;
        void Print() const;

        Function* CreateFunction(
            const SourceLocation& where,
            FnMode mode,
            const TypePtr& result,
            const TypePtr& self,
            const std::string& name,
            const std::string& ir_name,
            const std::vector<Param>& params,
            bool vararg,
            const Statement* body);

        void DIDeclareParam(
            const SourceLocation& where,
            size_t index,
            const TypePtr& type,
            const std::string& name,
            const LValuePtr& value);

        void DIDeclareVariable(
            const SourceLocation& where,
            const TypePtr& type,
            const std::string& name,
            const LValuePtr& value);

        // Value Stack Utility
        void StackPush(bool globalize = false);
        void StackPop();
        ValuePtr& operator[](const std::string& name);

        // Value Utility
        LValuePtr CreateInstance(const SourceLocation&, const TypePtr& type, const std::string& name = "");

        // Local destructors
        void ClearLocalDestructors();
        void AddLocalDestructor(const SourceLocation&, const LValuePtr& value);
        void RemoveLocalDtor(const ValuePtr& value);
        void GenLocalDestructors(const SourceLocation&);

        // Function Utility
        Function* GetFunction(const std::string& name, const FunctionTypePtr& type);
        Function* FindFunction(const std::string& name, const TypePtr& self, const std::vector<TypePtr>& args);
        Function* FindFunction(const std::string& name);
        Function* FindConstructor(const TypePtr& self, const std::vector<TypePtr>& args);
        Function* FindDestructor(const TypePtr& self);

        // Type Utility
        [[nodiscard]] TypePtr GetVoidTy() const;
        [[nodiscard]] TypePtr GetInt1Ty() const;
        [[nodiscard]] TypePtr GetInt8Ty() const;
        [[nodiscard]] TypePtr GetInt16Ty() const;
        [[nodiscard]] TypePtr GetInt32Ty() const;
        [[nodiscard]] TypePtr GetInt64Ty() const;
        [[nodiscard]] TypePtr GetFloat16Ty() const;
        [[nodiscard]] TypePtr GetFloat32Ty() const;
        [[nodiscard]] TypePtr GetFloat64Ty() const;
        [[nodiscard]] PointerTypePtr GetVoidPtrTy() const;
        [[nodiscard]] PointerTypePtr GetInt8PtrTy() const;

        [[nodiscard]] bool IsCallee() const;
        void SetCallee();
        void ClearCallee();

        ValuePtr& Self();

        [[nodiscard]] size_t GetArgCount() const;
        TypePtr& GetArg(size_t);
        std::vector<TypePtr>& GetArgs();

        TypePtr& GetResult();

    private:
        Context& m_Context;

        llvm::LLVMContext& m_IRContext;
        std::unique_ptr<llvm::Module> m_IRModule;
        std::unique_ptr<llvm::IRBuilder<>> m_IRBuilder;
        std::unique_ptr<llvm::DIBuilder> m_DIBuilder;

        llvm::DICompileUnit* m_CU;

        std::unique_ptr<llvm::FunctionPassManager> m_FPM;
        std::unique_ptr<llvm::LoopAnalysisManager> m_LAM;
        std::unique_ptr<llvm::FunctionAnalysisManager> m_FAM;
        std::unique_ptr<llvm::CGSCCAnalysisManager> m_CGAM;
        std::unique_ptr<llvm::ModuleAnalysisManager> m_MAM;
        std::unique_ptr<llvm::PassInstrumentationCallbacks> m_PIC;
        std::unique_ptr<llvm::StandardInstrumentations> m_SI;

        std::map<std::string, std::map<FunctionTypePtr, Function>> m_Functions;

        std::vector<StackFrame> m_Stack;
        StackFrame m_Frame;

        bool m_IsCallee = false;
        std::vector<TypePtr> m_Args;

        const bool m_Debug;
        const unsigned m_Optimization;
    };
}
