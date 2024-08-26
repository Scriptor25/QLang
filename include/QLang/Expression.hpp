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
	};

	struct BinaryExpression : Expression
	{
		BinaryExpression(const SourceLocation &, const std::string &operator_,
						 ExpressionPtr lhs, ExpressionPtr rhs);

		void Print(std::ostream &) const override;

		std::string Operator;
		ExpressionPtr LHS;
		ExpressionPtr RHS;
	};

	struct CallExpression : Expression
	{
		CallExpression(const SourceLocation &, ExpressionPtr callee,
					   std::vector<ExpressionPtr> &args);

		void Print(std::ostream &) const override;

		ExpressionPtr Callee;
		std::vector<ExpressionPtr> Args;
	};

	struct ConstIntExpression : Expression
	{
		ConstIntExpression(const SourceLocation &, uint64_t value);

		void Print(std::ostream &) const override;

		uint64_t Value;
	};

	struct ConstStringExpression : Expression
	{
		ConstStringExpression(const SourceLocation &, const std::string &value);

		void Print(std::ostream &) const override;

		std::string Value;
	};

	struct IndexExpression : Expression
	{
		IndexExpression(
			const SourceLocation &, ExpressionPtr array, ExpressionPtr index);

		void Print(std::ostream &) const override;

		ExpressionPtr Array;
		ExpressionPtr Index;
	};

	struct MemberExpression : Expression
	{
		MemberExpression(const SourceLocation &, ExpressionPtr object,
						 bool dereference, const std::string &member);

		void Print(std::ostream &) const override;

		ExpressionPtr Object;
		bool Dereference;
		std::string Member;
	};

	struct NameExpression : Expression
	{
		NameExpression(const SourceLocation &, const std::string &name);

		void Print(std::ostream &) const override;

		std::string Name;
	};

	struct UnaryExpression : Expression
	{
		UnaryExpression(const SourceLocation &, const std::string &operator_,
						ExpressionPtr operand, bool left);

		void Print(std::ostream &) const override;

		std::string Operator;
		ExpressionPtr Operand;
		bool Left;
	};
}
