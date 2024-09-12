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
            include_dirs.emplace_back(argv[++i]);
            continue;
        }
        if (arg == "-ea")
        {
            emit_ast = true;
            continue;
        }
        if (arg == "-ei")
        {
            emit_ir = true;
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

#ifdef _WIN32
        auto& [where_, name_, params_, is_callee_, value_] = context.GetMacro("SYSTEM_WINDOWS");
        name_ = "SYSTEM_WINDOWS";
#elifdef linux
        auto& [where_, name_, params_, is_callee_, value_] = context.GetMacro("SYSTEM_LINUX");
        name_ = "SYSTEM_LINUX";
#endif
        is_callee_ = false;
        value_ = "1";

        QLang::Builder builder(context, linker.IRContext(), module_name, filename, directory);
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
