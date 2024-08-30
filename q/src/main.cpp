#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Linker.hpp>
#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, const char **argv)
{
	std::vector<std::string> input_filenames;
	std::string output_filename = "a.out";

	for (size_t i = 1; i < argc; ++i)
	{
		std::string arg(argv[i]);

		if (arg == "-o")
		{
			output_filename = argv[++i];
			continue;
		}

		input_filenames.push_back(arg);
	}

	if (input_filenames.empty())
	{
		std::cerr << "no input files provided" << std::endl;
		return 1;
	}

	QLang::Linker linker;

	for (const auto &filename : input_filenames)
	{
		std::ifstream stream(filename);
		if (!stream) continue;

		QLang::Context context;
		QLang::Builder builder(context, linker.IRContext());
		QLang::Parser parser(builder, stream, filename);

		while (!parser.AtEof())
		{
			auto ptr = parser.Parse();
			if (!ptr) continue;

			std::cerr << ptr << std::endl;

			ptr->GenIRVoid(builder);
		}

		stream.close();
		linker.Link(builder);
	}

	linker.EmitObject(output_filename);
}
