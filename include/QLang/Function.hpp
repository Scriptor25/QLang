#pragma once

#include <QLang/QLang.hpp>
#include <llvm/IR/Function.h>
#include <string>

namespace QLang
{
	struct Function
	{
		RValuePtr AsValue(Builder &) const;

		std::string Name;
		FunctionTypePtr Type;
		llvm::Function *IR = nullptr;
		llvm::FunctionType *IRType = nullptr;
	};
}
