#pragma once

#include <QLang/QLang.hpp>
#include <string>
#include <vector>

namespace QLang
{
	class Type
	{
	public:
		static TypePtr Get(Context &, const std::string &);

	public:
		Type(Context &, const std::string &name, size_t size);
		virtual ~Type();

		Context &GetCtx() const;
		const std::string &GetName() const;
		size_t GetSize() const;

	private:
		Context &m_Ctx;
		std::string m_Name;
		size_t m_Size;
	};

	class PointerType : public Type
	{
	public:
		static PointerTypePtr Get(const TypePtr &base);

	public:
		PointerType(Context &, const std::string &name, const TypePtr &base);

		TypePtr GetBase() const;

	private:
		TypePtr m_Base;
	};

	class ArrayType : public Type
	{
	public:
		static ArrayTypePtr Get(const TypePtr &base, uint64_t length);

	public:
		ArrayType(Context &, const std::string &name, const TypePtr &base,
				  uint64_t length);

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
		static StructTypePtr Get(Context &, const std::string &name);
		static StructTypePtr Get(
			const std::string &name, std::vector<StructElement> &elements);

	public:
		StructType(Context &, const std::string &name, size_t size,
				   std::vector<StructElement> &elements);

		size_t GetElementCount() const;
		const StructElement &GetElement(size_t index) const;

	private:
		std::vector<StructElement> m_Elements;
	};

	class FunctionType : public Type
	{
	public:
		static FunctionTypePtr Get();
	};
}
