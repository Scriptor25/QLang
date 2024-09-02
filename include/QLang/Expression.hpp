#pragma once

#include <QLang/QLang.hpp>
#include <QLang/SourceLocation.hpp>
#include <QLang/Statement.hpp>
#include <vector>

namespace QLang
{
	struct Expression : Statement
	{
		Expression(const SourceLocation &);
		void GenIRVoid(Builder &) const override;

		virtual ValuePtr GenIR(Builder &) const = 0;
	};

	struct BinaryExpression : Expression
	{
		BinaryExpression(const SourceLocation &, const std::string &operator_,
						 ExpressionPtr lhs, ExpressionPtr rhs);

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;

		std::string Operator;
		ExpressionPtr LHS;
		ExpressionPtr RHS;
	};

	struct CallExpression : Expression
	{
		CallExpression(const SourceLocation &, ExpressionPtr callee,
					   std::vector<ExpressionPtr> &args);

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;

		ExpressionPtr Callee;
		std::vector<ExpressionPtr> Args;
	};

	struct ConstCharExpression : Expression
	{
		ConstCharExpression(const SourceLocation &, char value);

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;

		char Value;
	};

	struct ConstFloatExpression : Expression
	{
		ConstFloatExpression(const SourceLocation &, double value);

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;

		double Value;
	};

	struct ConstIntExpression : Expression
	{
		ConstIntExpression(const SourceLocation &, uint64_t value);

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;

		uint64_t Value;
	};

	struct ConstStringExpression : Expression
	{
		ConstStringExpression(const SourceLocation &, const std::string &value);

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;

		std::string Value;
	};

	struct NameExpression : Expression
	{
		NameExpression(const SourceLocation &, const std::string &name);

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;

		std::string Name;
	};

	struct TernaryExpression : Expression
	{
		TernaryExpression(const SourceLocation &, ExpressionPtr if_,
						  ExpressionPtr then, ExpressionPtr else_);

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;

		ExpressionPtr If;
		ExpressionPtr Then;
		ExpressionPtr Else;
	};

	struct UnaryExpression : Expression
	{
		UnaryExpression(const SourceLocation &, const std::string &operator_,
						ExpressionPtr operand, bool post);

		std::ostream &Print(std::ostream &) const override;
		ValuePtr GenIR(Builder &) const override;

		std::string Operator;
		ExpressionPtr Operand;
		bool Post;
	};
}
