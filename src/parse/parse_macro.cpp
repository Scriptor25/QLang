#include <QLang/Context.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Statement.hpp>
#include <string>
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

	auto &ref = m_Context.GetMacro(name);
	ref.Where = m_Token.Where;
	ref.Name = name;
	ref.Params = params;
	ref.IsCallee = is_callee;
	ref.Value = m_Token.Value;

	while (m_C >= 0 && m_C != '\n')
	{
		if (m_C == '\\')
		{
			m_C = Get();
			if (m_C == '\n') NewLine();
			else
				ref.Value += '\\';
		}

		ref.Value += static_cast<char>(m_C);
		m_C = Get();
	}
	m_C = Get();
	NewLine();
	Next();
}
