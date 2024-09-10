#pragma once

#include <QLang/QLang.hpp>
#include <QLang/SourceLocation.hpp>
#include <QLang/Type.hpp>
#include <vector>

namespace QLang
{
	struct Statement
	{
		explicit Statement(SourceLocation );
		virtual ~Statement();

		virtual std::ostream &Print(std::ostream &) const = 0;
		virtual void GenIRVoid(Builder &) const = 0;

		SourceLocation Where;
	};

	struct CompoundStatement : Statement
	{
		CompoundStatement(
			const SourceLocation &, std::vector<StatementPtr> &list);

		std::ostream &Print(std::ostream &) const override;
		void GenIRVoid(Builder &) const override;

		std::vector<StatementPtr> List;
	};

	struct DefFnStatement : Statement
	{
		DefFnStatement(
			const SourceLocation &, bool is_extern, FnMode mode,
			TypePtr result, TypePtr self, std::string name,
			const std::vector<Param> &params, bool vararg, StatementPtr body);

		std::ostream &Print(std::ostream &) const override;
		void GenIRVoid(Builder &) const override;

		[[nodiscard]] std::string GenName() const;

		bool IsExtern;
		FnMode Mode;
		TypePtr Result;
		TypePtr Self;
		std::string Name;
		std::vector<Param> Params;
		bool VarArg;
		StatementPtr Body;
	};

	struct DefVarStatement : Statement
	{
		DefVarStatement(
			const SourceLocation &, bool is_extern, const TypePtr &type,
			const std::string &name, StatementPtr init);

		DefVarStatement(
			const SourceLocation &, bool is_extern, const TypePtr &type,
			const std::string &name, std::vector<StatementPtr> args);

		DefVarStatement(
			const SourceLocation &, bool is_extern, TypePtr type,
			std::string name, ExpressionPtr init);

		DefVarStatement(
			const SourceLocation &, bool is_extern, TypePtr type,
			std::string name, std::vector<ExpressionPtr> args);

		std::ostream &Print(std::ostream &) const override;
		void GenIRVoid(Builder &) const override;

		bool IsExtern;
		TypePtr Type;
		std::string Name;
		ExpressionPtr Init;
		std::vector<ExpressionPtr> Args;
	};

	struct IfStatement : Statement
	{
		IfStatement(const SourceLocation &, StatementPtr if_, StatementPtr then,
					StatementPtr else_);

		IfStatement(const SourceLocation &, ExpressionPtr if_,
					StatementPtr then, StatementPtr else_);

		std::ostream &Print(std::ostream &) const override;
		void GenIRVoid(Builder &) const override;

		ExpressionPtr If;
		StatementPtr Then;
		StatementPtr Else;
	};

	struct ReturnStatement : Statement
	{
		ReturnStatement(const SourceLocation &, StatementPtr value);
		ReturnStatement(const SourceLocation &, ExpressionPtr value);

		std::ostream &Print(std::ostream &) const override;
		void GenIRVoid(Builder &) const override;

		ExpressionPtr Value;
	};

	struct WhileStatement : Statement
	{
		WhileStatement(
			const SourceLocation &, StatementPtr while_, StatementPtr loop);

		WhileStatement(
			const SourceLocation &, ExpressionPtr while_, StatementPtr loop);

		std::ostream &Print(std::ostream &) const override;
		void GenIRVoid(Builder &) const override;

		ExpressionPtr While;
		StatementPtr Loop;
	};
}
