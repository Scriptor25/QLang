#pragma once

#include <QLang/QLang.hpp>
#include <vector>

namespace QLang
{
	ValuePtr GenAdd(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenSub(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenMul(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenDiv(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenRem(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenAnd(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenOr(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenXor(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenEQ(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenNE(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenLE(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenGE(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenLT(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenGT(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenLAnd(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenLOr(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenLXor(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenShl(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenLShr(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenAShr(Builder &, const ValuePtr &, const ValuePtr &);

	ValuePtr GenSubscript(Builder &, const ValuePtr &, const ValuePtr &);
	ValuePtr GenCast(Builder &, const ValuePtr &, const TypePtr &);
	ValuePtr GenCall(Builder &, const ValuePtr &, const LValuePtr &,
					 const std::vector<ValuePtr> &);

	ValuePtr GenLNot(Builder &, const ValuePtr &);
	ValuePtr GenNot(Builder &, const ValuePtr &);
	ValuePtr GenNeg(Builder &, const ValuePtr &);

	ValuePtr GenReference(Builder &, const ValuePtr &);
	ValuePtr GenDereference(Builder &, const ValuePtr &);
}
