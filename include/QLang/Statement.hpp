#pragma once

#include <QLang/QLang.hpp>
#include <QLang/SourceLocation.hpp>
#include <QLang/Type.hpp>
#include <vector>

namespace QLang
{
	struct Statement
	{
		explicit Statement(const SourceLocation &);
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

	struct Param
	{
		TypePtr Type;
		std::string Name;
	};

	struct DefFnStatement : Statement
	{
		DefFnStatement(
			const SourceLocation &, FnMode mode, const TypePtr &result,
			const TypePtr &self, const std::string &name,
			const std::vector<Param> &params, bool vararg, StatementPtr body);

		std::ostream &Print(std::ostream &) const override;
		void GenIRVoid(Builder &) const override;

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
		DefVarStatement(const SourceLocation &, const TypePtr &type,
						const std::string &name, ExpressionPtr init);

		std::ostream &Print(std::ostream &) const override;
		void GenIRVoid(Builder &) const override;

		TypePtr Type;
		std::string Name;
		ExpressionPtr Init;
	};

	struct IfStatement : Statement
	{
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
		ReturnStatement(const SourceLocation &, ExpressionPtr value);

		std::ostream &Print(std::ostream &) const override;
		void GenIRVoid(Builder &) const override;

		ExpressionPtr Value;
	};

	struct WhileStatement : Statement
	{
		WhileStatement(
			const SourceLocation &, ExpressionPtr condition, StatementPtr loop);

		std::ostream &Print(std::ostream &) const override;
		void GenIRVoid(Builder &) const override;

		ExpressionPtr Condition;
		StatementPtr Loop;
	};
}
