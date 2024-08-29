#pragma once

#include <QLang/QLang.hpp>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <string>
#include <vector>

namespace QLang
{
	enum TypeId
	{
		TypeId_Void,
		TypeId_Int,
		TypeId_Float,
		TypeId_Pointer,
		TypeId_Reference,
		TypeId_Array,
		TypeId_Struct,
		TypeId_Function
	};

	class Type
	{
	public:
		static TypePtr Get(Context &, const std::string &);
		static size_t TypeDiff(Builder&, TypePtr, TypePtr);
		static TypePtr HigherOrder(const TypePtr &, const TypePtr &);

	public:
		virtual ~Type();
		virtual llvm::Type *GenIR(Builder &) const;

		Type(Context &, const std::string &name, TypeId id, size_t size);

		Context &GetCtx() const;
		std::string GetName() const;
		TypeId GetId() const;
		size_t GetSize() const;

		bool IsVoid() const;
		bool IsInt() const;
		bool IsFloat() const;
		bool IsPointer() const;
		bool IsReference() const;
		bool IsArray() const;
		bool IsStruct() const;
		bool IsFunction() const;

	private:
		Context &m_Ctx;
		std::string m_Name;
		TypeId m_Id;
		size_t m_Size;
	};

	class PointerType : public Type
	{
	public:
		static PointerTypePtr From(const TypePtr &);
		static PointerTypePtr Get(const TypePtr &base);

	public:
		PointerType(Context &, const std::string &name, const TypePtr &base);

		llvm::PointerType *GenIR(Builder &) const override;

		TypePtr GetBase() const;

	private:
		TypePtr m_Base;
	};

	class ReferenceType : public Type
	{
	public:
		static ReferenceTypePtr From(const TypePtr &);
		static ReferenceTypePtr Get(const TypePtr &base);

	public:
		ReferenceType(Context &, const std::string &name, const TypePtr &base);

		llvm::Type *GenIR(Builder &) const override;

		TypePtr GetBase() const;

	private:
		TypePtr m_Base;
	};

	class ArrayType : public Type
	{
	public:
		static ArrayTypePtr From(const TypePtr &);
		static ArrayTypePtr Get(const TypePtr &base, uint64_t length);

	public:
		ArrayType(Context &, const std::string &name, const TypePtr &base,
				  uint64_t length);

		llvm::ArrayType *GenIR(Builder &) const override;

		TypePtr GetBase() const;
		uint64_t GetLength() const;

	private:
		TypePtr m_Base;
		uint64_t m_Length;
	};

	struct StructElement
	{
		TypePtr Type;
		std::string Name;
		ExpressionPtr Init;
	};

	class StructType : public Type
	{
	public:
		static StructTypePtr From(const TypePtr &);
		static StructTypePtr Get(Context &, const std::string &name);
		static StructTypePtr Get(
			const std::string &name, std::vector<StructElement> &elements);

	public:
		StructType(
			Context &, const std::string &name, const std::string &struct_name,
			size_t size, std::vector<StructElement> &elements);

		llvm::StructType *GenIR(Builder &) const override;

		size_t GetElementCount() const;
		const StructElement &GetElement(size_t index) const;

	private:
		std::string m_StructName;
		std::vector<StructElement> m_Elements;
	};

	enum FnMode
	{
		FnMode_Func,
		FnMode_Ctor,
		FnMode_Dtor,
	};

	class FunctionType : public Type
	{
	public:
		static FunctionTypePtr From(const TypePtr &);
		static FunctionTypePtr FromPtr(const TypePtr &);
		static FunctionTypePtr Get(
			FnMode mode, const TypePtr &result, const TypePtr &self,
			const std::vector<TypePtr> &params, bool vararg);

	public:
		FunctionType(
			Context &, const std::string &name, FnMode mode,
			const TypePtr &result, const TypePtr &self,
			const std::vector<TypePtr> &params, bool vararg);

		llvm::FunctionType *GenIR(Builder &) const override;

		FnMode GetMode() const;
		TypePtr GetResult() const;
		TypePtr GetSelf() const;
		size_t GetParamCount() const;
		TypePtr GetParam(size_t i) const;
		bool IsVarArg() const;

	private:
		FnMode m_Mode;
		TypePtr m_Result;
		TypePtr m_Self;
		std::vector<TypePtr> m_Params;
		bool m_VarArg;
	};
}
