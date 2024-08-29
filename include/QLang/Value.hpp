#pragma once

#include <QLang/QLang.hpp>
#include <llvm/IR/Value.h>

namespace QLang
{
	class Value
	{
	public:
		virtual ~Value();
		virtual llvm::Value *Get() const = 0;

		Builder &GetBuilder() const;
		TypePtr GetType() const;
		llvm::Type *GetIRType() const;

	protected:
		Value(Builder &, const TypePtr &type);

	private:
		Builder &m_Builder;
		TypePtr m_Type;
		llvm::Type *m_IRType;
	};

	class RValue : public Value
	{
	public:
		static RValuePtr From(const ValuePtr &);
		static RValuePtr Create(
			Builder &, const TypePtr &type, llvm::Value *value);

	public:
		RValue(Builder &, const TypePtr &type, llvm::Value *value);

		llvm::Value *Get() const override;

	private:
		llvm::Value *m_Value;
	};

	class LValue : public Value
	{
	public:
		static LValuePtr From(const ValuePtr &);
		static LValuePtr Create(
			Builder &, const TypePtr &type, llvm::Value *ptr);
		static LValuePtr Alloca(
			Builder &, const TypePtr &type, llvm::Value *value = nullptr);

	public:
		LValue(Builder &, const TypePtr &type, llvm::Value *ptr);

		llvm::Value *Get() const override;

		llvm::Value *GetPtr() const;
		void Set(llvm::Value *value) const;

	private:
		llvm::Value *m_Ptr;
	};
}
