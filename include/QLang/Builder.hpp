#pragma once

#include <QLang/Function.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Value.hpp>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <map>
#include <memory>
#include <string>

namespace QLang
{
	class Builder
	{
	public:
		Builder(Context &, llvm::LLVMContext &);

		Context &GetContext() const;

		llvm::LLVMContext &IRContext() const;
		llvm::IRBuilder<> &IRBuilder() const;
		llvm::Module &IRModule() const;

		std::unique_ptr<llvm::Module> &IRModulePtr();

		void Optimize(llvm::Function *);

		// Value Stack Utility
		void StackPush();
		void StackPop();
		ValuePtr &operator[](const std::string &name);

		// Value Destructions
		std::vector<ValuePtr> &DestroyAtEnd();

		// Function Utility
		Function &GetFunction(
			const std::string &name, const FunctionTypePtr &type);
		Function *FindFunction(
			const std::string &name, const TypePtr &self,
			const std::vector<TypePtr> &args);
		Function *FindConstructor(
			const TypePtr &self, const std::vector<TypePtr> &args);
		Function *FindDestructor(const TypePtr &self);

		// Type Utility
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

		bool IsCallee();
		void SetCallee();
		void ClearCallee();

		ValuePtr &Self();

		void SetArgCount(size_t);
		size_t GetArgCount();
		TypePtr &GetArg(size_t);
		const std::vector<TypePtr> &GetArgs();

		TypePtr &GetResult();

	private:
		Context &m_Context;

		llvm::LLVMContext &m_IRContext;
		std::unique_ptr<llvm::IRBuilder<>> m_IRBuilder;
		std::unique_ptr<llvm::Module> m_IRModule;

		std::unique_ptr<llvm::FunctionPassManager> m_FPM;
		std::unique_ptr<llvm::LoopAnalysisManager> m_LAM;
		std::unique_ptr<llvm::FunctionAnalysisManager> m_FAM;
		std::unique_ptr<llvm::CGSCCAnalysisManager> m_CGAM;
		std::unique_ptr<llvm::ModuleAnalysisManager> m_MAM;
		std::unique_ptr<llvm::PassInstrumentationCallbacks> m_PIC;
		std::unique_ptr<llvm::StandardInstrumentations> m_SI;

		std::vector<std::map<std::string, ValuePtr>> m_Stack;
		std::map<std::string, ValuePtr> m_Values;

		std::vector<QLang::ValuePtr> m_DestroyAtEnd;

		std::map<std::string, std::map<FunctionTypePtr, Function>> m_Functions;

		bool m_IsCallee = false;
		ValuePtr m_Self;
		std::vector<TypePtr> m_Args;
		TypePtr m_Result;
	};
}
