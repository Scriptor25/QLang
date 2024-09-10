#pragma once

#include <QLang/QLang.hpp>
#include <QLang/Statement.hpp>
#include <vector>

namespace QLang
{
    struct Expression : Statement
    {
        explicit Expression(const SourceLocation&);
        void GenIRVoid(Builder&) const override;

        [[nodiscard]] virtual bool IsConstant() const;

        virtual ValuePtr GenIR(Builder&) const = 0;
        virtual ExpressionPtr Collapse() = 0;
    };

    ExpressionPtr Collapse(StatementPtr);
    ExpressionPtr Collapse(ExpressionPtr);

    struct BinaryExpression : Expression
    {
        BinaryExpression(const SourceLocation&, const std::string& operator_,
                         StatementPtr lhs, StatementPtr rhs);

        BinaryExpression(const SourceLocation&, std::string operator_,
                         ExpressionPtr lhs, ExpressionPtr rhs);

        [[nodiscard]] bool IsConstant() const override;

        std::ostream& Print(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;
        ExpressionPtr Collapse() override;

        std::string Operator;
        ExpressionPtr LHS;
        ExpressionPtr RHS;
    };

    struct CallExpression : Expression
    {
        CallExpression(const SourceLocation&, StatementPtr callee,
                       std::vector<StatementPtr> args);

        CallExpression(const SourceLocation&, ExpressionPtr callee,
                       std::vector<ExpressionPtr> args);

        std::ostream& Print(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;
        ExpressionPtr Collapse() override;

        ExpressionPtr Callee;
        std::vector<ExpressionPtr> Args;
    };

    struct CastExpression : Expression
    {
        CastExpression(
            const SourceLocation&, const TypePtr& dst, StatementPtr src);

        CastExpression(const SourceLocation&, TypePtr dst, ExpressionPtr src);

        std::ostream& Print(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;
        ExpressionPtr Collapse() override;

        TypePtr Dst;
        ExpressionPtr Src;
    };

    struct ConstCharExpression : Expression
    {
        ConstCharExpression(const SourceLocation&, char value);

        [[nodiscard]] bool IsConstant() const override;

        std::ostream& Print(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;
        ExpressionPtr Collapse() override;

        char Value;
    };

    struct ConstFloatExpression : Expression
    {
        ConstFloatExpression(const SourceLocation&, double value);

        [[nodiscard]] bool IsConstant() const override;

        std::ostream& Print(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;
        ExpressionPtr Collapse() override;

        double Value;
    };

    struct ConstIntExpression : Expression
    {
        ConstIntExpression(const SourceLocation&, uint64_t value);

        [[nodiscard]] bool IsConstant() const override;

        std::ostream& Print(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;
        ExpressionPtr Collapse() override;

        uint64_t Value;
    };

    struct ConstStringExpression : Expression
    {
        ConstStringExpression(const SourceLocation&, std::string value);

        [[nodiscard]] bool IsConstant() const override;

        std::ostream& Print(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;
        ExpressionPtr Collapse() override;

        std::string Value;
    };

    struct FunctionExpression : Expression
    {
        FunctionExpression(const SourceLocation&,
                           const std::vector<Param>& params,
                           bool vararg,
                           TypePtr result,
                           StatementPtr body);

        std::ostream& Print(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;
        ExpressionPtr Collapse() override;

        std::vector<Param> Params;
        bool VarArg;
        TypePtr Result;
        StatementPtr Body;
    };

    struct NameExpression : Expression
    {
        NameExpression(const SourceLocation&, std::string name);

        std::ostream& Print(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;
        ExpressionPtr Collapse() override;

        std::string Name;
    };

    struct TernaryExpression : Expression
    {
        TernaryExpression(const SourceLocation&, StatementPtr if_,
                          StatementPtr then, StatementPtr else_);

        TernaryExpression(const SourceLocation&, ExpressionPtr if_,
                          ExpressionPtr then, ExpressionPtr else_);

        [[nodiscard]] bool IsConstant() const override;

        std::ostream& Print(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;
        ExpressionPtr Collapse() override;

        ExpressionPtr If;
        ExpressionPtr Then;
        ExpressionPtr Else;
    };

    struct UnaryExpression : Expression
    {
        UnaryExpression(const SourceLocation&, const std::string& operator_,
                        StatementPtr operand, bool post);

        UnaryExpression(const SourceLocation&, std::string operator_,
                        ExpressionPtr operand, bool post);

        [[nodiscard]] bool IsConstant() const override;

        std::ostream& Print(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;
        ExpressionPtr Collapse() override;

        std::string Operator;
        ExpressionPtr Operand;
        bool Post;
    };
}
