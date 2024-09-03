#pragma once

#include <memory>
#include <vector>

namespace QLang
{
	class Context;
	class Parser;
	class Builder;
	class Linker;

	struct SourceLocation;
	struct Token;

	struct Macro;
	struct Function;

	class Type;
	typedef std::shared_ptr<Type> TypePtr;
	class PointerType;
	typedef std::shared_ptr<PointerType> PointerTypePtr;
	class ReferenceType;
	typedef std::shared_ptr<ReferenceType> ReferenceTypePtr;
	class ArrayType;
	typedef std::shared_ptr<ArrayType> ArrayTypePtr;
	class StructType;
	typedef std::shared_ptr<StructType> StructTypePtr;
	class FunctionType;
	typedef std::shared_ptr<FunctionType> FunctionTypePtr;

	class Value;
	typedef std::shared_ptr<Value> ValuePtr;
	class RValue;
	typedef std::shared_ptr<RValue> RValuePtr;
	class LValue;
	typedef std::shared_ptr<LValue> LValuePtr;

	struct Statement;
	typedef std::unique_ptr<Statement> StatementPtr;
	struct Expression;
	typedef std::unique_ptr<Expression> ExpressionPtr;

	template <typename T, typename U>
	std::unique_ptr<T> dyn_cast(std::unique_ptr<U> &&ptr)
	{
		if (T *p = dynamic_cast<T *>(ptr.get()))
		{
			ptr.release();
			return std::unique_ptr<T>(p);
		}
		return {};
	}

	template <typename T, typename U>
	std::unique_ptr<T> dyn_cast(std::unique_ptr<U> &ptr)
	{
		if (T *p = dynamic_cast<T *>(ptr.get()))
		{
			ptr.release();
			return std::unique_ptr<T>(p);
		}
		return {};
	}

	template <typename T, typename U>
	std::vector<std::unique_ptr<T>> dyn_cast(
		std::vector<std::unique_ptr<U>> &uv)
	{
		std::vector<std::unique_ptr<T>> tv(uv.size());
		for (size_t i = 0; i < tv.size(); ++i) tv[i] = dyn_cast<T, U>(uv[i]);
		return tv;
	}

	std::ostream &operator<<(std::ostream &, const SourceLocation &);
	std::ostream &operator<<(std::ostream &, const TypePtr &);
	std::ostream &operator<<(std::ostream &, const StatementPtr &);
	std::ostream &operator<<(std::ostream &, const ExpressionPtr &);

	void Print(const StatementPtr &);
}
