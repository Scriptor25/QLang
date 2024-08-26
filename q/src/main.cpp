#include <QLang/Parser.hpp>
#include <QLang/Statement.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, const char **argv)
{
	std::vector<std::string> input_filenames;
	std::string output_filename;

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

	for (const auto &filename : input_filenames)
	{
		std::ifstream stream(filename);
		if (!stream) continue;

		QLang::Parser parser(stream, filename);
		while (!parser.AtEof())
		{
			auto ptr = parser.Parse();
			if (!ptr) continue;
			ptr->Print(std::cout);
			std::cout << std::endl;
		}

		stream.close();
	}
}
