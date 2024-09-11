#pragma once

#include <string>
#include <llvm/IR/DebugInfo.h>
#include <QLang/QLang.hpp>

namespace QLang
{
    struct SourceLocation
    {
        [[nodiscard]] llvm::DILocation* GenDI(Builder&) const;

        std::string Filename;
        size_t Row = 1, Column = 0;
    };
}
