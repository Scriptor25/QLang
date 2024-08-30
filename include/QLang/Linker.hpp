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

		llvm::LLVMContext &IRContext() const;

		void Link(Builder &builder);
		void EmitObject(const std::string &filename);

	private:
		std::unique_ptr<llvm::LLVMContext> m_IRContext;
		std::unique_ptr<llvm::Module> m_IRModule;
	};
}
