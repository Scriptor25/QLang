#include <QLang/Context.hpp>
#include <QLang/Parser.hpp>
#include <string>
#include <vector>

void QLang::Parser::ParseMacro()
{
	Expect("macro");
	const auto name = Expect(TokenType_Name).Value;

	std::vector<std::string> params;
	const bool is_callee = !NextIfAt("=") && NextIfAt("(");

	if (is_callee)
		while (!NextIfAt(")"))
		{
			params.push_back(Expect(TokenType_Name).Value);
			if (!At(")")) Expect(",");
		}

	auto &[_where, _name, _params, _is_callee, _value]
		= m_Context.GetMacro(name);
	_where = m_Token.Where;
	_name = name;
	_params = params;
	_is_callee = is_callee;
	_value = m_Token.Value;

	while (m_C >= 0 && m_C != '\n')
	{
		if (m_C == '\\')
		{
			m_C = Get();
			if (m_C == '\n') NewLine();
			else
				_value += '\\';
		}

		_value += static_cast<char>(m_C);
		m_C = Get();
	}
	m_C = Get();
	NewLine();
	Next();
}
