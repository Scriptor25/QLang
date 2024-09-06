#pragma once

#include <QLang/QLang.hpp>
#include <QLang/Statement.hpp>
#include <vector>

namespace QLang
{
	struct Expression : Statement
	{
		explicit Expression(const SourceLocation &);
		void GenIRVoid(Builder &) const override;

		[[nodiscard]] virtual bool IsConstant() const;

		virtual ValuePtr GenIR(Builder &) const = 0;
		virtual ExpressionPtr Compress() = 0;
	};

	ExpressionPtr Compress(StatementPtr);
	ExpressionPtr Compress(ExpressionPtr);

	struct BinaryExpression : Expression
	{
		BinaryExpression(const SourceLocation &, const std::string &operator_,
						 StatementPtr lhs, StatementPtr rhs);

		BinaryExpression(const SourceLocation &, std::string operator_,
						 ExpressionPtr lhs, ExpressionPtr rhs);

		[[nodiscard]] bool IsConstant() const override;

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;
		ExpressionPtr Compress() override;

		std::string Operator;
		ExpressionPtr LHS;
		ExpressionPtr RHS;
	};

	struct CallExpression : Expression
	{
		CallExpression(const SourceLocation &, StatementPtr callee,
					   std::vector<StatementPtr> args);

		CallExpression(const SourceLocation &, ExpressionPtr callee,
					   std::vector<ExpressionPtr> args);

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;
		ExpressionPtr Compress() override;

		ExpressionPtr Callee;
		std::vector<ExpressionPtr> Args;
	};

	struct CastExpression : Expression
	{
		CastExpression(
			const SourceLocation &, const TypePtr &dst, StatementPtr src);

		CastExpression(const SourceLocation &, TypePtr dst, ExpressionPtr src);

		[[nodiscard]] bool IsConstant() const override;

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;
		ExpressionPtr Compress() override;

		TypePtr Dst;
		ExpressionPtr Src;
	};

	struct ConstCharExpression : Expression
	{
		ConstCharExpression(const SourceLocation &, char value);

		[[nodiscard]] bool IsConstant() const override;

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;
		ExpressionPtr Compress() override;

		char Value;
	};

	struct ConstFloatExpression : Expression
	{
		ConstFloatExpression(const SourceLocation &, double value);

		[[nodiscard]] bool IsConstant() const override;

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;
		ExpressionPtr Compress() override;

		double Value;
	};

	struct ConstIntExpression : Expression
	{
		ConstIntExpression(const SourceLocation &, uint64_t value);

		[[nodiscard]] bool IsConstant() const override;

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;
		ExpressionPtr Compress() override;

		uint64_t Value;
	};

	struct ConstStringExpression : Expression
	{
		ConstStringExpression(const SourceLocation &, std::string value);

		[[nodiscard]] bool IsConstant() const override;

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;
		ExpressionPtr Compress() override;

		std::string Value;
	};

	struct NameExpression : Expression
	{
		NameExpression(const SourceLocation &, std::string name);

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;
		ExpressionPtr Compress() override;

		std::string Name;
	};

	struct TernaryExpression : Expression
	{
		TernaryExpression(const SourceLocation &, StatementPtr if_,
						  StatementPtr then, StatementPtr else_);

		TernaryExpression(const SourceLocation &, ExpressionPtr if_,
						  ExpressionPtr then, ExpressionPtr else_);

		[[nodiscard]] bool IsConstant() const override;

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;
		ExpressionPtr Compress() override;

		ExpressionPtr If;
		ExpressionPtr Then;
		ExpressionPtr Else;
	};

	struct UnaryExpression : Expression
	{
		UnaryExpression(const SourceLocation &, const std::string &operator_,
						StatementPtr operand, bool post);

		UnaryExpression(const SourceLocation &, std::string operator_,
						ExpressionPtr operand, bool post);

		[[nodiscard]] bool IsConstant() const override;

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;
		ExpressionPtr Compress() override;

		std::string Operator;
		ExpressionPtr Operand;
		bool Post;
	};
}
