#include <QLang/Statement.hpp>
#include <QLang/Type.hpp>
#include <iostream>

QLang::DefFnStatement::DefFnStatement(
	const SourceLocation &where, FnMode mode, const TypePtr &result,
	const TypePtr &self, const std::string &name,
	const std::vector<Param> &params, bool vararg, StatementPtr body)
	: Statement(where), Mode(mode), Result(result), Self(self), Name(name),
	  Params(params), VarArg(vararg), Body(std::move(body))
{
}

void QLang::DefFnStatement::Print(std::ostream &stream) const
{
	stream << "def ";
	if (Mode == FnMode_Func)
	{
		stream << Result->GetName() << ' ';
		if (Self) stream << Self->GetName() << ':';
	}
	else if (Mode == FnMode_Ctor) { stream << '+'; }
	else if (Mode == FnMode_Dtor) { stream << '-'; }
	stream << Name << '(';
	for (size_t i = 0; i < Params.size(); ++i)
	{
		if (i > 0) stream << ", ";
		stream << Params[i].Type->GetName();
		if (!Params[i].Name.empty()) stream << ' ' << Params[i].Name;
	}
	if (VarArg)
	{
		if (!Params.empty()) stream << ", ";
		stream << '?';
	}
	stream << ')';
	if (Body)
	{
		stream << ' ';
		Body->Print(stream);
	}
}
