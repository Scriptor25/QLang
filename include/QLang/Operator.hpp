#pragma once

#include <QLang/QLang.hpp>
#include <vector>

namespace QLang
{
    ValuePtr GenAdd(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenSub(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenMul(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenDiv(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenRem(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenAnd(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenOr(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenXor(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenEQ(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenNE(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenLE(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenGE(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenLT(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenGT(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenLAnd(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenLOr(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenLXor(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenShl(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenLShr(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenAShr(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);

    ValuePtr GenMember(const SourceLocation&, Builder&, ValuePtr, bool, const std::string&);
    LValuePtr GenMember(const SourceLocation&, Builder&, const LValuePtr&, size_t);
    ValuePtr GenSubscript(const SourceLocation&, Builder&, const ValuePtr&, const ValuePtr&);
    ValuePtr GenCast(const SourceLocation&, Builder&, const ValuePtr&, const TypePtr&);
    ValuePtr GenCall(
        const SourceLocation&,
        Builder&,
        const ValuePtr&,
        const LValuePtr&,
        const std::vector<ValuePtr>&);

    ValuePtr GenInc(const SourceLocation&, Builder&, const ValuePtr&);
    ValuePtr GenDec(const SourceLocation&, Builder&, const ValuePtr&);
    ValuePtr GenLNot(const SourceLocation&, Builder&, const ValuePtr&);
    ValuePtr GenNot(const SourceLocation&, Builder&, const ValuePtr&);
    ValuePtr GenNeg(const SourceLocation&, Builder&, const ValuePtr&);
    ValuePtr GenRef(Builder&, const ValuePtr&);
    ValuePtr GenDeref(Builder&, const ValuePtr&);
}
