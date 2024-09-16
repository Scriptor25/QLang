#pragma once

#include <QLang/Macro.hpp>
#include <QLang/QLang.hpp>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace QLang
{
    class Context
    {
    public:
        Context();

        TypePtr& GetType(const std::string&);

        void AddIncludeDir(const std::filesystem::path&);
        bool FindInIncludeDirs(std::filesystem::path& dest, const std::string& filename);
        bool AddParsed(const std::filesystem::path&);

        Macro& GetMacro(const std::string& name);
        [[nodiscard]] bool HasMacro(const std::string& name) const;

        void SetTemplateParams(const std::vector<std::string>& params);
        void ClearTemplateParams();
        [[nodiscard]] bool HasTemplateParams() const;
        TypePtr GetTemplateParam(const std::string& name);

    private:
        std::map<std::string, TypePtr> m_Types;
        std::map<std::string, Macro> m_Macros;

        std::vector<std::filesystem::path> m_IncludeDirs;
        std::vector<std::filesystem::path> m_Parsed;

        std::map<std::string, TypePtr> m_Params;
    };
}
