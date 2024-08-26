#pragma once

#include <memory>

namespace QLang
{
	class Context;
	class Parser;
	class Builder;

	struct SourceLocation;
	struct Token;

	class Type;
	typedef std::shared_ptr<Type> TypePtr;
	class PointerType;
	typedef std::shared_ptr<PointerType> PointerTypePtr;
	class ArrayType;
	typedef std::shared_ptr<ArrayType> ArrayTypePtr;
	class StructType;
	typedef std::shared_ptr<StructType> StructTypePtr;
	class FunctionType;
	typedef std::shared_ptr<FunctionType> FunctionTypePtr;

	struct Statement;
	typedef std::unique_ptr<Statement> StatementPtr;
	struct Expression;
	typedef std::unique_ptr<Expression> ExpressionPtr;

	template <typename T, typename U>
	std::unique_ptr<T> dynamic_pointer_cast(std::unique_ptr<U> &&ptr)
	{
		if (T *p = dynamic_cast<T *>(ptr.get()))
		{
			ptr.release();
			return std::unique_ptr<T>(p);
		}
		return {};
	}
}
