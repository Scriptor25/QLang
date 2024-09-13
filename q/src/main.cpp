#include <QLang/Builder.hpp>
#include <QLang/Context.hpp>
#include <QLang/Linker.hpp>
#include <QLang/Parser.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Statement.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, const char** argv)
{
    std::vector<std::string> input_filenames;
    std::vector<std::string> include_dirs;
    std::string output_filename = "a.out";
    bool emit_ast = false;
    bool emit_ir = false;
    bool optimize = false;

    for (size_t i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);

        if (arg == "--help" || arg == "-h")
        {
            std::cout << "QLang [v1.0.0]" << std::endl;
            std::cout << "USAGE" << std::endl;
            std::cout << '\t' << "qlang [OPTIONS] <filename>..." << std::endl;
            std::cout << "OPTIONS" << std::endl;
            std::cout << '\t' << "--help,     -h" << '\t' << " show help info" << std::endl;
            std::cout << '\t' << "--version,  -v" << '\t' << " show version info" << std::endl;
            std::cout << '\t' << "--output,   -o" << '\t' << " specify the output filename" << std::endl;
            std::cout << '\t' << "--include,  -I" << '\t' << " add a path to the include directories" << std::endl;
            std::cout << '\t' << "--emit-ir,  -R" << '\t' << " set the emit ir flag" << std::endl;
            std::cout << '\t' << "--emit-ast, -A" << '\t' << " set the emit ast flag" << std::endl;
            std::cout << '\t' << "--optimize, -O" << '\t' << " set the optimize flag" << std::endl;
            return 0;
        }
        if (arg == "--version" || arg == "-v")
        {
            std::cout << "QLang [v1.0.0]" << std::endl;
            continue;
        }
        if (arg == "--output" || arg == "-o")
        {
            output_filename = argv[++i];
            continue;
        }
        if (arg == "--include" || arg == "-I")
        {
            include_dirs.emplace_back(argv[++i]);
            continue;
        }
        if (arg == "--emit-ir" || arg == "-R")
        {
            emit_ast = true;
            continue;
        }
        if (arg == "--emit-ast" || arg == "-A")
        {
            emit_ir = true;
            continue;
        }
        if (arg == "--optimize" || arg == "-O")
        {
            optimize = true;
            continue;
        }

        input_filenames.push_back(arg);
    }

    if (input_filenames.empty())
    {
        std::cerr << "no input files provided; use '--help' for more information" << std::endl;
        return 1;
    }

    QLang::Linker linker;

    for (const auto& input_filename : input_filenames)
    {
        std::ifstream stream(input_filename);
        if (!stream)
        {
            std::cerr << "failed to open file " << input_filename << std::endl;
            continue;
        }

        std::string directory = std::filesystem::path(input_filename).parent_path().string();
        std::string filename = std::filesystem::path(input_filename).filename().string();
        std::string module_name = std::filesystem::path(input_filename).replace_extension().filename().string();

        QLang::Context context;
        for (const auto& dir : include_dirs)
            context.AddIncludeDir(dir);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
        auto& [where_, name_, params_, is_callee_, value_] = context.GetMacro("SYSTEM_WINDOWS");
        name_ = "SYSTEM_WINDOWS";
#elif defined(linux) || defined(_linux) || defined(__linux)
        auto& [where_, name_, params_, is_callee_, value_] = context.GetMacro("SYSTEM_LINUX");
        name_ = "SYSTEM_LINUX";
#endif
        is_callee_ = false;
        value_ = "1";

        QLang::Builder builder(
            context,
            linker.IRContext(),
            module_name,
            filename,
            directory,
            optimize);
        QLang::Parser parser(
            builder,
            stream,
            {.Filename = input_filename},
            [&](const QLang::StatementPtr& ptr)
            {
                if (emit_ast)
                    std::cerr << ptr << std::endl;
                ptr->GenIRVoid(builder);
            });

        parser.Parse();
        stream.close();

        linker.Link(builder);
    }

    if (emit_ir) linker.Print();
    linker.EmitObject(output_filename);
}
