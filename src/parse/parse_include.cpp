#include <QLang/Context.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Token.hpp>
#include <filesystem>
#include <fstream>

void QLang::Parser::ParseInclude()
{
	Expect("include");
	auto filename = Expect(TokenType_String).Value;

	auto path
		= std::filesystem::path(m_Where.Filename).parent_path() / filename;
	if (m_Context.AddParsed(path)) return;

	std::ifstream stream(path);
	Parser parser(m_Builder, stream, filename);
	while (!parser.AtEof())
	{
		auto ptr = parser.Parse();
		if (!ptr) continue;

		ptr->GenIRVoid(m_Builder);
	}
	stream.close();
}
