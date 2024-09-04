#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Linker.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Statement.hpp>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, const char **argv)
{
	std::vector<std::string> input_filenames;
	std::vector<std::string> include_dirs;
	std::string output_filename = "a.out";

	for (size_t i = 1; i < argc; ++i)
	{
		std::string arg(argv[i]);

		if (arg == "-o")
		{
			output_filename = argv[++i];
			continue;
		}

		if (arg == "-I")
		{
			include_dirs.push_back(argv[++i]);
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

		std::string modulename
			= std::filesystem::path(filename)
				  .replace_extension()
				  .filename()
				  .string();

		QLang::Context context;
		for (const auto &dir : include_dirs) context.AddIncludeDir(dir);

		QLang::Builder builder(context, linker.IRContext(), modulename);
		QLang::Parser parser(
			builder, stream, { .Filename = filename },
			[&](QLang::StatementPtr ptr)
			{
				std::cerr << ptr << std::endl;
				ptr->GenIRVoid(builder);
			});

		parser.Parse();
		stream.close();

		builder.Print();
		linker.Link(builder);
	}

	linker.EmitObject(output_filename);
}
