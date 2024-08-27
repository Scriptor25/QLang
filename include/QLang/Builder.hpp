#pragma once

#include <QLang/QLang.hpp>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace QLang
{
	class Builder
	{
	public:
		explicit Builder(Context &);

		llvm::LLVMContext &IRContext() const;
		llvm::IRBuilder<> &IRBuilder() const;
		llvm::Module &IRModule() const;

		void Push();
		void Pop();

		TypePtr GetVoidTy() const;
		TypePtr GetInt1Ty() const;
		TypePtr GetInt8Ty() const;
		TypePtr GetInt16Ty() const;
		TypePtr GetInt32Ty() const;
		TypePtr GetInt64Ty() const;
		TypePtr GetFloat16Ty() const;
		TypePtr GetFloat32Ty() const;
		TypePtr GetFloat64Ty() const;

		PointerTypePtr GetVoidPtrTy() const;
		PointerTypePtr GetInt8PtrTy() const;

	private:
		std::unique_ptr<llvm::LLVMContext> m_IRContext;
		std::unique_ptr<llvm::IRBuilder<>> m_IRBuilder;
		std::unique_ptr<llvm::Module> m_IRModule;

		Context &m_Context;
	};
}
