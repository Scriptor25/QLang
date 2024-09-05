#include <QLang/Context.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Token.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

void QLang::Parser::ParseInclude()
{
	Expect("include");
	auto filename = Expect(TokenType_String).Value;

	std::filesystem::path filepath;
	if (!m_Context.FindInIncludeDirs(filepath, filename))
		filepath
			= std::filesystem::path(m_Where.Filename).parent_path() / filename;
	if (m_Context.AddParsed(filepath)) return;

	std::ifstream stream(filepath);
	if (!stream)
	{
		std::cerr << "failed to open include file '" << filename << "' ("
				  << filepath << ")" << std::endl;
		return;
	}

	Parser parser(
		m_Builder, stream, { .Filename = filepath.string() }, m_Callback);
	parser.Parse();
	stream.close();
}
