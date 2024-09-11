#pragma once

#include <string>
#include <vector>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <QLang/QLang.hpp>

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
        static TypePtr Get(Context&, const std::string&);
        static size_t TypeDiff(Builder&, TypePtr, TypePtr);
        static TypePtr HigherOrder(const TypePtr&, const TypePtr&);

        virtual ~Type();
        virtual llvm::Type* GenIR(Builder&);
        virtual llvm::DIType* GenDI(Builder&);

        Type(Context&, std::string name, TypeId id, size_t size);

        [[nodiscard]] Context& GetCtx() const;
        [[nodiscard]] std::string GetName() const;
        [[nodiscard]] TypeId GetId() const;
        [[nodiscard]] size_t GetSize() const;

        [[nodiscard]] bool IsVoid() const;
        [[nodiscard]] bool IsInt() const;
        [[nodiscard]] bool IsFloat() const;
        [[nodiscard]] bool IsPointer() const;
        [[nodiscard]] bool IsReference() const;
        [[nodiscard]] bool IsArray() const;
        [[nodiscard]] bool IsStruct() const;
        [[nodiscard]] bool IsFunction() const;

        [[nodiscard]] bool IsFunctionPointer() const;

    private:
        Context& m_Ctx;
        std::string m_Name;
        TypeId m_Id;
        size_t m_Size;

        llvm::Type* m_IR = nullptr;
        llvm::DIType* m_DI = nullptr;
    };

    class PointerType : public Type
    {
    public:
        static PointerTypePtr From(const TypePtr&);
        static PointerTypePtr Get(const TypePtr& base);

        PointerType(Context&, const std::string& name, TypePtr base);

        llvm::PointerType* GenIR(Builder&) override;
        llvm::DIType* GenDI(Builder&) override;

        [[nodiscard]] TypePtr GetBase() const;

    private:
        TypePtr m_Base;

        llvm::PointerType* m_IR = nullptr;
        llvm::DIType* m_DI = nullptr;
    };

    class ReferenceType : public Type
    {
    public:
        static ReferenceTypePtr From(const TypePtr&);
        static ReferenceTypePtr Get(const TypePtr& base);

        ReferenceType(Context&, const std::string& name, TypePtr base);

        llvm::Type* GenIR(Builder&) override;
        llvm::DIType* GenDI(Builder&) override;

        [[nodiscard]] TypePtr GetBase() const;

    private:
        TypePtr m_Base;

        llvm::Type* m_IR = nullptr;
        llvm::DIType* m_DI = nullptr;
    };

    class ArrayType : public Type
    {
    public:
        static ArrayTypePtr From(const TypePtr&);
        static ArrayTypePtr Get(const TypePtr& base, uint64_t length);

        ArrayType(Context&, const std::string& name, const TypePtr& base, uint64_t length);

        llvm::ArrayType* GenIR(Builder&) override;
        llvm::DIType* GenDI(Builder&) override;

        [[nodiscard]] TypePtr GetBase() const;
        [[nodiscard]] uint64_t GetLength() const;

    private:
        TypePtr m_Base;
        uint64_t m_Length;

        llvm::ArrayType* m_IR = nullptr;
        llvm::DIType* m_DI = nullptr;
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
        static StructTypePtr From(const TypePtr&);
        static StructTypePtr Get(Context&, const std::string& name);
        static StructTypePtr Get(const std::string& name, std::vector<StructElement>& elements);

        StructType(
            Context&,
            const std::string& name,
            std::string struct_name,
            size_t size,
            std::vector<StructElement>& elements);

        llvm::StructType* GenIR(Builder&) override;
        llvm::DIType* GenDI(Builder&) override;

        [[nodiscard]] size_t GetElementCount() const;
        [[nodiscard]] const StructElement& GetElement(size_t index) const;

    private:
        std::string m_StructName;
        std::vector<StructElement> m_Elements;

        llvm::StructType* m_IR = nullptr;
        llvm::DIType* m_DI = nullptr;
    };

    class FunctionType : public Type
    {
    public:
        static FunctionTypePtr From(const TypePtr&);
        static FunctionTypePtr FromPtr(const TypePtr&);
        static FunctionTypePtr Get(
            FnMode mode,
            const TypePtr& result,
            const TypePtr& self,
            const std::vector<TypePtr>& params, bool vararg);

        FunctionType(
            Context&,
            const std::string& name,
            FnMode mode,
            TypePtr result,
            TypePtr self,
            const std::vector<TypePtr>& params,
            bool vararg);

        llvm::FunctionType* GenIR(Builder&) override;
        llvm::DISubroutineType* GenDI(Builder&) override;

        [[nodiscard]] FnMode GetMode() const;
        [[nodiscard]] TypePtr GetResult() const;
        [[nodiscard]] TypePtr GetSelf() const;
        [[nodiscard]] size_t GetParamCount() const;
        [[nodiscard]] TypePtr GetParam(size_t i) const;
        [[nodiscard]] bool IsVarArg() const;

    private:
        FnMode m_Mode;
        TypePtr m_Result;
        TypePtr m_Self;
        std::vector<TypePtr> m_Params;
        bool m_VarArg;

        llvm::FunctionType* m_IR = nullptr;
        llvm::DISubroutineType* m_DI = nullptr;
    };
}
