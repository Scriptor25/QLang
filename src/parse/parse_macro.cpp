#include <QLang/Context.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Statement.hpp>
#include <sstream>
#include <vector>

void QLang::Parser::ParseMacro()
{
	Expect("macro");
	auto name = Expect(TokenType_Name).Value;

	std::vector<std::string> params;
	bool is_callee = NextIfAt("(");

	if (is_callee)
		while (!NextIfAt(")"))
		{
			params.push_back(Expect(TokenType_Name).Value);
			if (!At(")")) Expect(",");
		}

	auto value = Parse();

	auto &ref = m_Context.GetMacro(name);
	ref.Where = value->Where;
	ref.Name = name;
	ref.Params = params;
	ref.IsCallee = is_callee;

	std::stringstream stream;
	value->Print(stream);
	stream.flush();
	ref.Value = stream.str();

	/*
	macro min(a, b) ((a) < (b) ? (a) : (b))

	...

	min(123, 456 + 789) -> ((123) < (456 + 789) ? (123) : (456 + 789))
	*/

	/*
	macro init(name) Init_\name()

	...

	init(ABC) -> Init_ABC()
	*/

	/*
	<name>  -> <arg>
	\<name> -> <argname>
	*/
}
