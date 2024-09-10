#pragma once

#include <QLang/QLang.hpp>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <string>

namespace QLang
{
    class Linker
    {
    public:
        Linker();

        [[nodiscard]] llvm::LLVMContext& IRContext() const;

        void Print() const;

        void Link(Builder& builder);
        void EmitObject(const std::string& filename) const;

    private:
        std::unique_ptr<llvm::LLVMContext> m_IRContext;
        std::unique_ptr<llvm::Module> m_IRModule;
    };
}
