#include <filesystem>
#include <fstream>
#include <iostream>
#include <QLang/Context.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Token.hpp>

void QLang::Parser::ParseCDInclude()
{
    Expect("#include");
    const auto filename = Expect(TokenType_String).Value;

    std::filesystem::path filepath;
    if (!m_Context.FindInIncludeDirs(filepath, filename))
        filepath = std::filesystem::path(m_State.Where.Filename).parent_path() / filename;
    if (m_Context.AddParsed(filepath)) return;

    const auto stream = std::make_shared<std::ifstream>(filepath);
    if (!*stream)
    {
        std::cerr << "failed to open include file '" << filename << "' (" << filepath << ")" << std::endl;
        return;
    }

    Parser parser(m_Builder, stream, filepath.string(), m_Callback);
    parser.Parse();
    stream->close();
}
